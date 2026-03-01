//@007 七. 主菜单 Widget 实现
//     功能: 初始化按钮绑定、Host/Join 操作、服务器列表展示

#include "MainMenu.h"

#include "UObject/ConstructorHelpers.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/PanelWidget.h"

#include "ServerRow.h"

//----------  

//<1> 构造函数 — 加载服务器行蓝图类
UMainMenu::UMainMenu(const FObjectInitializer& ObjectInitializer)
{
	//[1] 查找 WBP_ServerRow 蓝图类  /Game/MenuSystem/WBP_ServerRow
	ConstructorHelpers::FClassFinder<UUserWidget> ServerRowBPClass(TEXT("/Game/MenuSystem/WBP_ServerRow"));
	if (!ensure(ServerRowBPClass.Class != nullptr)) return;

	//[2] 缓存服务器行蓝图类，供服务器列表动态创建
	ServerRowClass = ServerRowBPClass.Class;
}

//----------  

//<2> Initialize — 绑定主菜单按钮事件
bool UMainMenu::Initialize()
{
	//[1] 调用父类初始化
	bool Success = Super::Initialize();
	if (!Success) return false;

	//[2] 绑定 Host 菜单相关按钮
	if (!ensure(HostButton != nullptr)) return false;
	HostButton->OnClicked.AddDynamic(this, &UMainMenu::OpenHostMenu);

	if (!ensure(CancelHostMenuButton != nullptr)) return false;
	CancelHostMenuButton->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);

	if (!ensure(ConfirmHostMenuButton != nullptr)) return false;
	ConfirmHostMenuButton->OnClicked.AddDynamic(this, &UMainMenu::HostServer);

	//[3] 绑定 Join 菜单相关按钮
	if (!ensure(JoinButton != nullptr)) return false;
	JoinButton->OnClicked.AddDynamic(this, &UMainMenu::OpenJoinMenu);

	if (!ensure(QuitButton != nullptr)) return false;
	QuitButton->OnClicked.AddDynamic(this, &UMainMenu::QuitPressed);

	if (!ensure(CancelJoinMenuButton != nullptr)) return false;
	CancelJoinMenuButton->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);

	if (!ensure(ConfirmJoinMenuButton != nullptr)) return false;
	ConfirmJoinMenuButton->OnClicked.AddDynamic(this, &UMainMenu::JoinServer);

	return true;
}

//----------  

//<3> OpenHostMenu — 切换到主机创建子页面
void UMainMenu::OpenHostMenu()
{
	if (!ensure(MenuSwitcher != nullptr)) return;
	if (!ensure(HostMenu != nullptr)) return;
	MenuSwitcher->SetActiveWidget(HostMenu);
}

//----------  

//<4> HostServer — 读取输入并发起 Host 会话
void UMainMenu::HostServer()
{
	if (MenuInterface != nullptr)
	{
		FString ServerName = ServerHostName->Text.ToString();
		MenuInterface->Host(ServerName);
	}
}

//----------  

//<5> SetServerList — 刷新服务器列表 UI
void UMainMenu::SetServerList(TArray<FServerData> ServerNames)
{
	//[1] 校验运行环境与模板类
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;
	if (!ensure(ServerList != nullptr)) return;
	if (!ensure(ServerRowClass != nullptr)) return;

	ServerList->ClearChildren();

	//[2] 遍历数据并动态生成服务器行
	uint32 i = 0;
	for (const FServerData& ServerData : ServerNames)
	{
		UServerRow* Row = CreateWidget<UServerRow>(World, ServerRowClass);
		if (!ensure(Row != nullptr)) return;

		Row->ServerName->SetText(FText::FromString(ServerData.Name));
		Row->HostUser->SetText(FText::FromString(ServerData.HostUsername));

		FString FractionText = FString::Printf(TEXT("%d/%d"), ServerData.CurrentPlayers, ServerData.MaxPlayers);
		Row->ConnectionFraction->SetText(FText::FromString(FractionText));
		Row->Setup(this, i);
		++i;

		ServerList->AddChild(Row);
	}
}

//----------  

//<6> SelectIndex — 记录当前选中的服务器索引
void UMainMenu::SelectIndex(uint32 Index)
{
	SelectedIndex = Index;
	UpdateChildren();
}

//----------  

//<7> UpdateChildren — 更新所有服务器行的选中状态
void UMainMenu::UpdateChildren()
{
	if (!ensure(ServerList != nullptr)) return;

	for (int32 i = 0; i < ServerList->GetChildrenCount(); ++i)
	{
		auto Row = Cast<UServerRow>(ServerList->GetChildAt(i));
		if (Row != nullptr)
		{
			Row->Selected = (SelectedIndex.IsSet() && SelectedIndex.GetValue() == i);
		}
	}
}

//----------  

//<8> JoinServer — 加入当前选中的服务器会话
void UMainMenu::JoinServer()
{
	if (SelectedIndex.IsSet() && MenuInterface != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected index %d."), SelectedIndex.GetValue());
		MenuInterface->Join(SelectedIndex.GetValue());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected index not set."));
	}
}

//----------  

//<9> OpenJoinMenu — 切换到加入页面并触发会话搜索
void UMainMenu::OpenJoinMenu()
{
	if (!ensure(MenuSwitcher != nullptr)) return;
	if (!ensure(JoinMenu != nullptr)) return;
	MenuSwitcher->SetActiveWidget(JoinMenu);

	if (MenuInterface != nullptr)
	{
		MenuInterface->RefreshServerList();
	}
}

//----------  

//<10> OpenMainMenu — 切换回主页面
void UMainMenu::OpenMainMenu()
{
	if (!ensure(MenuSwitcher != nullptr)) return;
	if (!ensure(MainMenu != nullptr)) return;
	MenuSwitcher->SetActiveWidget(MainMenu);
}

//----------  

//<11> QuitPressed — 通过控制台命令退出游戏
void UMainMenu::QuitPressed()
{
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	PlayerController->ConsoleCommand(TEXT("quit"));
}
