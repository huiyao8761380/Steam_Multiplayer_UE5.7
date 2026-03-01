//@010 十. 主游戏模式实现
//     功能: 通过 ConstructorHelpers 查找默认 Pawn 蓝图并赋值给 DefaultPawnClass

#include "PuzzlePlatformsGameMode.h"
#include "PuzzlePlatformsCharacter.h"
#include "UObject/ConstructorHelpers.h"

//----------  

//<1> 构造函数 — 设置默认 Pawn 类为 ThirdPersonCharacter 蓝图
APuzzlePlatformsGameMode::APuzzlePlatformsGameMode()
{
	//[1] 设置默认 Pawn 为 ThirdPersonCharacter 蓝图（UE5 模板路径）
	//    资源路径: /Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		//[2] 设置默认 Pawn 类  GameFramework/GameModeBase.h
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
