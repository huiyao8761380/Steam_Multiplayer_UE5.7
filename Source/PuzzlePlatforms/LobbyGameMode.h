//@011 十一. 大厅房间 GameMode
//     功能: 等待玩家加入，当玩家数达到要求时倒计时后跳转至游戏关卡
//     继承: APuzzlePlatformsGameMode

#pragma once

#include "CoreMinimal.h"
#include "PuzzlePlatformsGameMode.h"
#include "LobbyGameMode.generated.h"

// 大厅关卡专用 GameMode：管理玩家计数与开局倒计时
UCLASS()
class PUZZLEPLATFORMS_API ALobbyGameMode : public APuzzlePlatformsGameMode
{
	GENERATED_BODY()

public:
	void PostLogin(APlayerController* NewPlayer) override;	//(1) 玩家登录时计数

	void Logout(AController* Exiting) override;	//(2) 玩家退出时减少计数

private:
	void StartGame();		//(3) 倒计时结束后启动游戏

	uint32 NumberOfPlayers = 0;		//(4) 当前玩家数量

	FTimerHandle GameStartTimer;	//(5) 游戏启动倒计时器
};
