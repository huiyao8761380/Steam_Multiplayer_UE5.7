//@008 八. 游戏内暂停菜单 Widget
//     功能: 提供「返回大厅」和「退出游戏」按钮，通过 IMenuInterface 实现跳转

#pragma once

#include "CoreMinimal.h"
#include "MenuWidget.h"
#include "InGameMenu.generated.h"

// 游戏内暂停菜单：处理继续游戏与返回主菜单交互
UCLASS()
class PUZZLEPLATFORMS_API UInGameMenu : public UMenuWidget
{
	GENERATED_BODY()
protected:
	virtual bool Initialize();		//(1) 初始化按钮绑定

private:
	UPROPERTY(meta = (BindWidget))
	class UButton* CancelButton;	//(2) 取消按钮

	UPROPERTY(meta = (BindWidget))
	class UButton* QuitButton;		//(3) 退出按钮

	UFUNCTION()
	void CancelPressed();		//(4) 关闭菜单

	UFUNCTION()
	void QuitPressed();		//(5) 返回主菜单
};
