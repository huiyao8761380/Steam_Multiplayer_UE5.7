//@008 八. 暂停菜单 Widget 实现
//     功能: 绑定 Cancel/Quit 按钮点击事件，调用 IMenuInterface 方法跳转/退出

#include "InGameMenu.h"

#include "Components/Button.h"

//----------  

//<1> Initialize — 初始化按钮点击绑定
bool UInGameMenu::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success) return false;

	//[1] 绑定取消按钮 → 关闭菜单
	if (!ensure(CancelButton != nullptr)) return false;
	CancelButton->OnClicked.AddDynamic(this, &UInGameMenu::CancelPressed);

	//[2] 绑定退出按钮 → 返回主菜单
	if (!ensure(QuitButton != nullptr)) return false;
	QuitButton->OnClicked.AddDynamic(this, &UInGameMenu::QuitPressed);

	return true;
}

//----------  

//<2> CancelPressed — 关闭暂停菜单
void UInGameMenu::CancelPressed()
{
	//[1] 调用 Teardown 恢复游戏  MenuWidget.h
	Teardown();
}


//----------  

//<3> QuitPressed — 返回主菜单
void UInGameMenu::QuitPressed()
{
	//[1] 先关闭菜单，再调用 MenuInterface 返回主菜单  MenuInterface.h
	if (MenuInterface != nullptr) {
		Teardown();
		MenuInterface->LoadMainMenu();
	}
}
