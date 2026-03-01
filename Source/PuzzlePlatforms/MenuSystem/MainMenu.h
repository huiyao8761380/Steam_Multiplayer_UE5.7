//@007 七. 主菜单 Widget
//     功能: 主页面（Host/Join）、服务器列表展示、子面板切换
//     依赖: UMenuWidget 基类、IMenuInterface（通过 GameInstance 实现）

#pragma once

#include "CoreMinimal.h"
#include "MenuWidget.h"
#include "MainMenu.generated.h"

USTRUCT()
struct FServerData 
{
	GENERATED_BODY()

	FString Name;			//(1) 服务器名称
	uint16 CurrentPlayers;	//(2) 当前玩家数
	uint16 MaxPlayers;		//(3) 最大玩家数
	FString HostUsername;	//(4) 房主用户名
};

// 主菜单 Widget：承载 Host/Join/Quit 与服务器列表选择逻辑
UCLASS()
class PUZZLEPLATFORMS_API UMainMenu : public UMenuWidget
{
	GENERATED_BODY()
public:
	UMainMenu(const FObjectInitializer & ObjectInitializer);	//(1) 构造函数，加载 ServerRow 蓝图类

	void SetServerList(TArray<FServerData> ServerNames);		//(2) 设置服务器列表数据

	void SelectIndex(uint32 Index);		//(3) 选中指定索引的服务器

protected:
	virtual bool Initialize();		//(4) 初始化按钮绑定

private:
	TSubclassOf<class UUserWidget> ServerRowClass;		//(5) ServerRow 蓝图类引用

	UPROPERTY(meta = (BindWidget))
	class UButton* HostButton;		//(6) 主机按钮
	
	UPROPERTY(meta = (BindWidget))
	class UButton* JoinButton;		//(7) 加入按钮

	UPROPERTY(meta = (BindWidget))
	class UButton* QuitButton;		//(8) 退出按钮

	UPROPERTY(meta = (BindWidget))
	class UButton* CancelJoinMenuButton;		//(9) 取消加入按钮

	UPROPERTY(meta = (BindWidget))
	class UButton* ConfirmJoinMenuButton;		//(10) 确认加入按钮

	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher;		//(11) 菜单切换器

	UPROPERTY(meta = (BindWidget))
	class UWidget* MainMenu;		//(12) 主页面

	UPROPERTY(meta = (BindWidget))
	class UWidget* JoinMenu;		//(13) 加入页面

	UPROPERTY(meta = (BindWidget))
	class UWidget* HostMenu;		//(14) 主机页面

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* ServerHostName;		//(15) 服务器名称输入框

	UPROPERTY(meta = (BindWidget))
	class UButton* CancelHostMenuButton;		//(16) 取消主机按钮

	UPROPERTY(meta = (BindWidget))
	class UButton* ConfirmHostMenuButton;		//(17) 确认主机按钮

	UPROPERTY(meta = (BindWidget))
	class UPanelWidget* ServerList;		//(18) 服务器列表容器

	UFUNCTION()
	void HostServer();		//(19) 创建服务器

	UFUNCTION()
	void JoinServer();		//(20) 加入选中服务器


	UFUNCTION()
	void OpenHostMenu();	//(21) 打开主机页面

	UFUNCTION()
	void OpenJoinMenu();	//(22) 打开加入页面

	UFUNCTION()
	void OpenMainMenu();	//(23) 返回主页面

	UFUNCTION()
	void QuitPressed();		//(24) 退出游戏

	TOptional<uint32> SelectedIndex;	//(25) 当前选中的服务器索引

	void UpdateChildren();		//(26) 更新子项选中状态
};
