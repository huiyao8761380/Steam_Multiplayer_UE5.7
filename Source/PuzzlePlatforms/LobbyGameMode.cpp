//@011 十一. 大厅 GameMode 实现
//     功能: 玩家进入时计数，达到人数后启动倒计时，调用 GameInstance 跳转至游戏关卡

#include "LobbyGameMode.h"

#include "TimerManager.h"

#include "PuzzlePlatformsGameInstance.h"

//----------  

//<1> PostLogin — 玩家登录时计数，达到人数后启动倒计时
void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	//[1] 调用父类 PostLogin  GameFramework/GameModeBase.h
	Super::PostLogin(NewPlayer);

	//→[2] 服务器端增加玩家计数
	++NumberOfPlayers;

	//[3] 达到 2 人后启动 10 秒倒计时
	if (NumberOfPlayers >= 2)
	{
		GetWorldTimerManager().SetTimer(GameStartTimer, this, &ALobbyGameMode::StartGame, 10);
	}
}

//----------  

//<2> Logout — 玩家退出时减少计数
void ALobbyGameMode::Logout(AController* Exiting)
{
	//[1] 调用父类 Logout  GameFramework/GameModeBase.h
	Super::Logout(Exiting);

	//→[2] 服务器端减少玩家计数
	--NumberOfPlayers;
}

//----------  

//<3> StartGame — 倒计时结束后启动游戏，跳转至游戏地图
void ALobbyGameMode::StartGame()
{
	//[1] 获取 GameInstance 并启动 Session  PuzzlePlatformsGameInstance.h
	auto GameInstance = Cast<UPuzzlePlatformsGameInstance>(GetGameInstance());

	if (GameInstance == nullptr) return;

	GameInstance->StartSession();

	//[2] 获取 World 并执行无缝跳转
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	//[3] 启用无缝跳转并切换到游戏地图
	bUseSeamlessTravel = true;
	World->ServerTravel("/Game/PuzzlePlatforms/Maps/Game?listen");
}
