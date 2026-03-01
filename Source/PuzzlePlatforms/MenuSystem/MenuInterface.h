//@005 五. 菜单功能接口
//     功能: 定义 GameInstance 必须实现的菜单操作接口（Host/Join/FindSessions 等）
//     实现类: UPuzzlePlatformsGameInstance

#pragma once

#include "CoreMinimal.h"
#include "MenuInterface.generated.h"

// 菜单接口 UInterface 壳类型，用于 UE 反射系统
UINTERFACE(MinimalAPI)
class UMenuInterface : public UInterface
{
	GENERATED_BODY()
};

// 菜单业务接口：由 GameInstance 实现，供各类 UI 菜单调用
class PUZZLEPLATFORMS_API IMenuInterface
{
	GENERATED_BODY()

public:
	virtual void Host(FString ServerName) = 0;		//(1) 创建服务器会话
	virtual void Join(uint32 Index) = 0;			//(2) 加入指定索引的服务器
	virtual void LoadMainMenu() = 0;				//(3) 返回主菜单地图
	virtual void RefreshServerList() = 0;			//(4) 刷新服务器列表

};
