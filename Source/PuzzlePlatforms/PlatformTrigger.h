//@003 三. 平台触发器 Actor
//     功能: 检测 Overlap 事件，通知关联的 MovingPlatform 激活/停止
//     组件: UBoxComponent 作为触碰检测区域

//※ 2. 虚幻5操作流程：
//说明: APlatformTrigger 继承自 AActor，需要在编辑器中手动配置触发平台列表
//注意事项: PlatformsToTrigger 数组在细节面板中拖入对应的 AMovingPlatform Actor 引用即可
//UE 操作步骤:
//   1. 将 BP_PlatformTrigger（继承 APlatformTrigger）放置在场景中
//   2. 选中 BP_PlatformTrigger，在细节面板 → Platforms To Trigger 中点击 +
//   3. 将场景中对应的 BP_MovingPlatform Actor 拖入数组元素槽
//   4. 设置 BoxComponent 的碰撞大小（细节面板 → Shape → Box Extent）

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlatformTrigger.generated.h"

UCLASS()
class PUZZLEPLATFORMS_API APlatformTrigger : public AActor
{
	GENERATED_BODY()

public:
	//设置该 Actor 属性的默认值
	APlatformTrigger();		//(1) 构造函数，创建 BoxComponent 并绑定 Overlap 委托

protected:
	virtual void BeginPlay() override;	//(2) 游戏开始时调用

public:
	virtual void Tick(float DeltaTime) override;	//(3) 每帧调用（当前空实现）

private:
	//触碰检测体积，设为 VisibleAnywhere 方便在编辑器中观察
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* TriggerVolume;		//(4) Box 触发体积组件

	//进入该触发器时需激活的所有平台
	UPROPERTY(EditAnywhere)
	TArray<class AMovingPlatform*> PlatformsToTrigger;		//(5) 关联的可移动平台数组

	// -------- Overlap 委托回调，必须标记 UFUNCTION() --------

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);	//(6) 有物体进入触发区域时调用

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);	//(7) 物体离开触发区域时调用
};
