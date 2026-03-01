//@012 十二. 核心游戏实例实现
//     功能: 管理 Steam/NULL 子系统 Session（创建/销毁/搜索/加入）

#include "PuzzlePlatformsGameInstance.h"

#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"

#include "MenuSystem/MainMenu.h"
#include "MenuSystem/MenuWidget.h"

const static FName SESSION_NAME = TEXT("Game");
const static FName SERVER_NAME_SETTINGS_KEY = TEXT("ServerName");

//----------  

//<1> 构造函数 — 加载主菜单与游戏内菜单蓝图类
UPuzzlePlatformsGameInstance::UPuzzlePlatformsGameInstance(const FObjectInitializer& ObjectInitializer)
{
	//[1] 加载主菜单蓝图类  /Game/MenuSystem/WBP_MainMenu
	ConstructorHelpers::FClassFinder<UUserWidget> MenuBPClass(TEXT("/Game/MenuSystem/WBP_MainMenu"));
	if (!ensure(MenuBPClass.Class != nullptr)) return;
	MenuClass = MenuBPClass.Class;

	//[2] 加载游戏内菜单蓝图类  /Game/MenuSystem/WBP_InGameMenu
	ConstructorHelpers::FClassFinder<UUserWidget> InGameMenuBPClass(TEXT("/Game/MenuSystem/WBP_InGameMenu"));
	if (!ensure(InGameMenuBPClass.Class != nullptr)) return;
	InGameMenuClass = InGameMenuBPClass.Class;
}

//----------  

//<2> Init — 初始化 OnlineSubsystem 并绑定 Session 回调
void UPuzzlePlatformsGameInstance::Init()
{
	//[1] 获取当前子系统（Steam/NULL）
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found no subsystem"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Found subsystem %s"), *Subsystem->GetSubsystemName().ToString());
	SessionInterface = Subsystem->GetSessionInterface();
	if (!SessionInterface.IsValid()) return;

	//[2] 绑定会话生命周期回调
	SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnCreateSessionComplete);
	SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnDestroySessionComplete);
	SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnFindSessionsComplete);
	SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnJoinSessionComplete);
}

//----------  

//<3> LoadMenuWidget — 创建并显示主菜单
void UPuzzlePlatformsGameInstance::LoadMenuWidget()
{
	if (!ensure(MenuClass != nullptr)) return;

	Menu = CreateWidget<UMainMenu>(this, MenuClass);
	if (!ensure(Menu != nullptr)) return;

	Menu->Setup();
	Menu->SetMenuInterface(this);
}

//----------  

//<4> InGameLoadMenu — 创建并显示游戏内暂停菜单
void UPuzzlePlatformsGameInstance::InGameLoadMenu()
{
	if (!ensure(InGameMenuClass != nullptr)) return;

	UMenuWidget* InGameMenu = CreateWidget<UMenuWidget>(this, InGameMenuClass);
	if (!ensure(InGameMenu != nullptr)) return;

	InGameMenu->Setup();
	InGameMenu->SetMenuInterface(this);
}

//----------  

//<5> Host — 记录目标服务器名并创建/重建会话
void UPuzzlePlatformsGameInstance::Host(FString ServerName)
{
	DesiredServerName = ServerName;

	if (!SessionInterface.IsValid()) return;

	auto ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);
	if (ExistingSession != nullptr)
	{
		SessionInterface->DestroySession(SESSION_NAME);
	}
	else
	{
		CreateSession();
	}
}

//----------  

//<6> OnDestroySessionComplete — 销毁旧会话后继续创建新会话
void UPuzzlePlatformsGameInstance::OnDestroySessionComplete(FName SessionName, bool Success)
{
	if (Success)
	{
		CreateSession();
	}
}

//----------  

//<7> CreateSession — 组装会话参数并发起创建
void UPuzzlePlatformsGameInstance::CreateSession()
{
	if (!SessionInterface.IsValid()) return;

	//[1] 重新获取子系统，决定 LAN/Online 匹配模式
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("No online subsystem in CreateSession"));
		return;
	}

	//[2] 配置会话参数
	FOnlineSessionSettings SessionSettings;
	SessionSettings.bIsLANMatch = (Subsystem->GetSubsystemName() == "NULL");
	SessionSettings.NumPublicConnections = 5;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.bUseLobbiesIfAvailable = true;
	SessionSettings.Set(SERVER_NAME_SETTINGS_KEY, DesiredServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	//[3] 发起会话创建
	SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);
}

//----------  

//<8> OnCreateSessionComplete — 会话创建成功后切换到 Lobby 地图
void UPuzzlePlatformsGameInstance::OnCreateSessionComplete(FName SessionName, bool Success)
{
	if (!Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not create session"));
		return;
	}

	if (Menu != nullptr)
	{
		Menu->Teardown();
	}

	UEngine* Engine = GetEngine();
	if (!ensure(Engine != nullptr)) return;

	Engine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Hosting"));

	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	World->ServerTravel(TEXT("/Game/PuzzlePlatforms/Maps/Lobby?listen"));
}

//----------  

//<9> RefreshServerList — 创建查询对象并开始搜索会话
void UPuzzlePlatformsGameInstance::RefreshServerList()
{
	if (!SessionInterface.IsValid()) return;

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	if (!SessionSearch.IsValid()) return;

	SessionSearch->MaxSearchResults = 100;
	SessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
	UE_LOG(LogTemp, Warning, TEXT("Starting Find Session"));

	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

//----------  

//<10> OnFindSessionsComplete — 将搜索结果转换为 UI 展示数据
void UPuzzlePlatformsGameInstance::OnFindSessionsComplete(bool Success)
{
	if (!Success || !SessionSearch.IsValid() || Menu == nullptr) return;

	UE_LOG(LogTemp, Warning, TEXT("Finished Find Session"));

	TArray<FServerData> ServerNames;
	for (const FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found session names: %s"), *SearchResult.GetSessionIdStr());

		FServerData Data;
		Data.MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections;
		Data.CurrentPlayers = Data.MaxPlayers - SearchResult.Session.NumOpenPublicConnections;
		Data.HostUsername = SearchResult.Session.OwningUserName;

		FString ServerName;
		if (SearchResult.Session.SessionSettings.Get(SERVER_NAME_SETTINGS_KEY, ServerName))
		{
			Data.Name = ServerName;
		}
		else
		{
			Data.Name = TEXT("Could not find name.");
		}

		ServerNames.Add(Data);
	}

	Menu->SetServerList(ServerNames);
}

//----------  

//<11> Join — 按索引加入会话
void UPuzzlePlatformsGameInstance::Join(uint32 Index)
{
	if (!SessionInterface.IsValid()) return;
	if (!SessionSearch.IsValid()) return;
	if (!SessionSearch->SearchResults.IsValidIndex(static_cast<int32>(Index))) return;

	if (Menu != nullptr)
	{
		Menu->Teardown();
	}

	SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[Index]);
}

//----------  

//<12> OnJoinSessionComplete — 解析地址并执行客户端跳转
void UPuzzlePlatformsGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (!SessionInterface.IsValid()) return;
	if (Result != EOnJoinSessionCompleteResult::Success) return;

	FString Address;
	if (!SessionInterface->GetResolvedConnectString(SessionName, Address))
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not get connect string."));
		return;
	}

	UEngine* Engine = GetEngine();
	if (!ensure(Engine != nullptr)) return;

	Engine->AddOnScreenDebugMessage(0, 5, FColor::Green, FString::Printf(TEXT("Joining %s"), *Address));

	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
}

//----------  

//<13> StartSession — 启动当前会话（Lobby -> Game 前调用）
void UPuzzlePlatformsGameInstance::StartSession()
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->StartSession(SESSION_NAME);
	}
}

//----------  

//<14> LoadMainMenu — 返回主菜单地图
void UPuzzlePlatformsGameInstance::LoadMainMenu()
{
	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	PlayerController->ClientTravel(TEXT("/Game/MenuSystem/MainMenu"), ETravelType::TRAVEL_Absolute);
}
