//@010 十. 主游戏模式
//     功能: 设置默认 Pawn 类为 ThirdPersonCharacter 蓝图，作为所有游戏关卡的基础 GameMode

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PuzzlePlatformsGameMode.generated.h"

UCLASS(minimalapi)
class APuzzlePlatformsGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APuzzlePlatformsGameMode();		//(1) 构造函数，设置默认 Pawn 类
};



