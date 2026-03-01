//@012 十二. 核心游戏实例
// 功能: 管理 OnlineSubsystem Session（创建/搜索/加入/销毁）并驱动菜单系统
// 说明: 该类实现 IMenuInterface，供主菜单与游戏内菜单调用联机逻辑

//③  PuzzlePlatformsGameInstance.h
//说明: 需要在项目设置中将 UPuzzlePlatformsGameInstance 设置为默认 GameInstance。
//注意事项: 未设置时 Session 功能不会生效；Steam 测试需启用 OnlineSubsystemSteam 插件。
//UE 操作步骤:
//  1. 编辑 -> 项目设置 -> 地图和模式 -> Game Instance Class -> 选择 PuzzlePlatformsGameInstance
//  2. 编辑 -> 插件 -> 搜索 Online Subsystem Steam -> 勾选启用 -> 重启编辑器
//  3. Config/DefaultEngine.ini 确认存在:
//     [OnlineSubsystem]
//     DefaultPlatformService=Steam
//     [/Script/Engine.GameEngine]
//     +NetDriverDefinitions=(DefName="GameNetDriver",DriverClassName="OnlineSubsystemSteam.SteamNetDriver",DriverClassNameFallback="OnlineSubsystemUtils.IpNetDriver")

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"
// UE5.7 迁移说明: OnlineSessionInterface 需要从 Interfaces 目录包含
#include "Interfaces/OnlineSessionInterface.h"

#include "MenuSystem/MenuInterface.h"
#include "PuzzlePlatformsGameInstance.generated.h"

UCLASS()
class PUZZLEPLATFORMS_API UPuzzlePlatformsGameInstance : public UGameInstance, public IMenuInterface
{
	GENERATED_BODY()

public:
	UPuzzlePlatformsGameInstance(const FObjectInitializer& ObjectInitializer); // (1) 构造函数，加载菜单蓝图类

	virtual void Init(); // (2) 初始化 OnlineSubsystem 并绑定 Session 回调

	UFUNCTION(BlueprintCallable)
	void LoadMenuWidget(); // (3) 加载并显示主菜单 Widget

	UFUNCTION(BlueprintCallable)
	void InGameLoadMenu(); // (4) 加载并显示游戏内菜单

	UFUNCTION(Exec)
	void Host(FString ServerName) override; // (5) 创建服务器会话并跳转至大厅地图

	UFUNCTION(Exec)
	void Join(uint32 Index) override; // (6) 加入指定索引的会话

	void StartSession(); // (7) 启动已创建会话

	virtual void LoadMainMenu() override; // (8) 返回主菜单地图

	void RefreshServerList() override; // (9) 刷新服务器列表

private:
	TSubclassOf<class UUserWidget> MenuClass; // (10) 主菜单蓝图类引用
	TSubclassOf<class UUserWidget> InGameMenuClass; // (11) 游戏内菜单蓝图类引用

	class UMainMenu* Menu; // (12) 主菜单 Widget 实例

	IOnlineSessionPtr SessionInterface; // (13) Session 接口指针
	TSharedPtr<class FOnlineSessionSearch> SessionSearch; // (14) 会话搜索对象

	// -------- Session 回调 --------
	void OnCreateSessionComplete(FName SessionName, bool Success); // (15) 创建会话完成回调
	void OnDestroySessionComplete(FName SessionName, bool Success); // (16) 销毁会话完成回调
	void OnFindSessionsComplete(bool Success); // (17) 搜索会话完成回调
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result); // (18) 加入会话完成回调

	FString DesiredServerName; // (19) 用户输入的服务器名称
	void CreateSession(); // (20) 创建新会话
};