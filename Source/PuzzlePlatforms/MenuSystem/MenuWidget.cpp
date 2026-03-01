//@006 六. 所有菜单 Widget 基类实现
//     功能: Setup 显示菜单并锁定 UI 输入模式, Teardown 隐藏菜单并恢复游戏输入模式

#include "MenuWidget.h"

//----------  

//<1> Setup — 显示菜单并锁定 UI 输入模式
void UMenuWidget::Setup()
{
	//[1] 将 Widget 添加到视口
	this->AddToViewport();

	//[2] 获取 World 和 PlayerController  Engine/World.h
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	//[3] 设置 UI Only 输入模式，锁定鼠标到 Widget
	FInputModeUIOnly InputModeData;
	InputModeData.SetWidgetToFocus(this->TakeWidget());
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	PlayerController->SetInputMode(InputModeData);

	//[4] 显示鼠标光标
	PlayerController->bShowMouseCursor = true;
}

//----------  

//<2> Teardown — 隐藏菜单并恢复游戏输入模式
void UMenuWidget::Teardown()
{
	//[1] 从视口移除 Widget
	this->RemoveFromViewport();

	//[2] 获取 World 和 PlayerController  Engine/World.h
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	//[3] 恢复 Game Only 输入模式
	FInputModeGameOnly InputModeData;
	PlayerController->SetInputMode(InputModeData);

	//[4] 隐藏鼠标光标
	PlayerController->bShowMouseCursor = false;
}

//----------  

//<3> SetMenuInterface — 设置菜单接口实现
void UMenuWidget::SetMenuInterface(IMenuInterface* InMenuInterface)
{
	//[1] 保存接口指针，供子类调用网络操作  MenuInterface.h
	this->MenuInterface = InMenuInterface;
}
