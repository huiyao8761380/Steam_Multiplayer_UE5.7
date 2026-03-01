//@009 九. 服务器列表行 Widget 实现
//     功能: 初始化父菜单和索引，点击时通知父菜单选中当前行

#include "ServerRow.h"

#include "Components/Button.h"

#include "MainMenu.h"

//----------  

//<1> Setup — 初始化父菜单和索引，绑定点击事件
void UServerRow::Setup(class UMainMenu* InParent, uint32 InIndex)
{
	//[1] 保存父菜单和索引
	Parent = InParent;
	Index = InIndex;

	//[2] 绑定按钮点击事件  Components/Button.h
	RowButton->OnClicked.AddDynamic(this, &UServerRow::OnClicked);
}

//----------  

//<2> OnClicked — 点击时通知父菜单选中当前行
void UServerRow::OnClicked()
{
	//[1] 调用父菜单的 SelectIndex 方法  MainMenu.h
	Parent->SelectIndex(Index);
}
