//@006 六. 所有菜单 Widget 的基类
//     功能: 提供 Setup/Teardown 方法，控制菜单显示/隐藏及输入模式切换
//     子类: UMainMenu, UInGameMenu

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuInterface.h"

#include "MenuWidget.generated.h"

// 菜单基类 Widget：统一处理显示/隐藏与输入模式切换
UCLASS()
class PUZZLEPLATFORMS_API UMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void Setup();		//(1) 显示菜单并锁定 UI 输入模式
	void Teardown();	//(2) 隐藏菜单并恢复游戏输入模式

	void SetMenuInterface(IMenuInterface* MenuInterface);	//(3) 设置菜单接口实现

protected:
	IMenuInterface* MenuInterface;		//(4) 菜单接口指针，由 GameInstance 实现
};
