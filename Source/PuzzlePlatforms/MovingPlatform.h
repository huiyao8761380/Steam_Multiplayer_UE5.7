//@002 二. 可移动平台 Actor
//     功能: 服务器权威控制的往返移动平台，支持通过触发器计数激活/停止
//     继承: AStaticMeshActor, 网络组件可复制到客户端

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "MovingPlatform.generated.h"

/**
 * 可往返移动的静态网格体平台
 * 仅在服务器端执行运动逻辑，通过 SetReplicateMovement 同步至所有客户端
 */
UCLASS()
class PUZZLEPLATFORMS_API AMovingPlatform : public AStaticMeshActor
{
	GENERATED_BODY()

public:
	AMovingPlatform();		//(1) 构造函数，设置 Tick 与网络复制

	virtual void BeginPlay() override;	//(2) 游戏开始时缓存起点/终点世界坐标

	virtual void Tick(float DeltaTime) override;	//(3) 每帧在服务器上推动平台移动

	// -------- 可在编辑器中配置的属性 --------

	UPROPERTY(EditAnywhere)
	float Speed = 20;			//(4) 平台移动速度（单位/秒）

	// MakeEditWidget=true 使 TargetLocation 在视口中显示可拖拽控件
	UPROPERTY(EditAnywhere, Meta = (MakeEditWidget = true))
	FVector TargetLocation;		//(5) 相对于 Actor 自身的目标偏移位置

	// -------- 触发器激活接口 --------

	void AddActiveTrigger();	//(6) 触发器进入时调用，增加激活计数
	void RemoveActiveTrigger();	//(7) 触发器离开时调用，减少激活计数

private:
	FVector GlobalTargetLocation;	//(8) 缓存的世界空间目标坐标
	FVector GlobalStartLocation;	//(9) 缓存的世界空间起始坐标

	// 当 ActiveTriggers > 0 时平台才会移动；默认为 1 允许平台无触发器时自动移动
	UPROPERTY(EditAnywhere)
	int ActiveTriggers = 1;		//(10) 当前激活的触发器数量
};
