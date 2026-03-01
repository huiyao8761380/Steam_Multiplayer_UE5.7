//@003 三. 平台触发器 Actor 实现
//     功能: 初始化 BoxComponent，绑定进入/离开 Overlap 委托，通知关联平台

#include "PlatformTrigger.h"
#include "Components/BoxComponent.h"
#include "MovingPlatform.h"

//----------  

//<1> 构造函数 — 创建 BoxComponent 并绑定 Overlap 委托
APlatformTrigger::APlatformTrigger()
{
	//[1] 允许 Tick（当前为空实现，保留扩展性）
	PrimaryActorTick.bCanEverTick = true;

	//[2] 创建 BoxComponent 作为触碰检测根组件
	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(FName("TriggerVolume"));
	if (!ensure(TriggerVolume != nullptr)) return;

	//[3] 设为根组件，所有 Overlap 检测均通过此组件触发
	RootComponent = TriggerVolume;

	//[4] 绑定进入/离开 Overlap 动态委托  Components/BoxComponent.h
	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &APlatformTrigger::OnOverlapBegin);
	TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &APlatformTrigger::OnOverlapEnd);
}

//----------  

//<2> BeginPlay — 游戏开始时调用（当前无额外逻辑）
void APlatformTrigger::BeginPlay()
{
	//[1] 调用父类 BeginPlay  GameFramework/Actor.h
	Super::BeginPlay();
}

//----------  

//<3> Tick — 每帧调用（触发器无需每帧逻辑，保留接口）
void APlatformTrigger::Tick(float DeltaTime)
{
	//[1] 调用父类 Tick  GameFramework/Actor.h
	Super::Tick(DeltaTime);
}

//----------  

//<4> OnOverlapBegin — 有物体进入触发区域时，通知关联平台增加激活计数
void APlatformTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	//[1] 遍历所有关联平台，调用 AddActiveTrigger  MovingPlatform.h
	for (AMovingPlatform* Platform : PlatformsToTrigger)
	{
		Platform->AddActiveTrigger();
	}
}

//----------  

//<5> OnOverlapEnd — 物体离开触发区域时，通知关联平台减少激活计数
void APlatformTrigger::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//[1] 遍历所有关联平台，调用 RemoveActiveTrigger  MovingPlatform.h
	for (AMovingPlatform* Platform : PlatformsToTrigger)
	{
		Platform->RemoveActiveTrigger();
	}
}
