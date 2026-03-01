//@002 二. 可移动平台 Actor 实现
//     功能: 服务器权威控制往返移动，根据触发器计数决定是否运动
//     网络: SetReplicates 在构造函数设置，BeginPlay 内设置移动复制

#include "MovingPlatform.h"

//----------  

//<1> 构造函数 — 初始化 Tick、设置可移动性、开启网络复制
AMovingPlatform::AMovingPlatform()
{
	//[1] 启用 Tick
	PrimaryActorTick.bCanEverTick = true;

	//[2] 设置 StaticMesh 可移动，否则在运行时无法改变位置
	SetMobility(EComponentMobility::Movable);

	//[3] UE5 推荐: 在构造函数中开启复制，而非 BeginPlay 中的 HasAuthority 判断内
	//    这样客户端连接时便能正确接收该 Actor 的数据
	SetReplicates(true);
}

//----------  

//<2> BeginPlay — 缓存世界空间起点与终点坐标
void AMovingPlatform::BeginPlay()
{
	//[1] 调用父类 BeginPlay  GameFramework/Actor.h
	Super::BeginPlay();

	//→[2] 仅服务器端开启移动复制（客户端会自动跟随服务器数据）
	if (HasAuthority())
	{
		SetReplicateMovement(true);
	}

	//[3] 记录 Actor 在世界中的初始位置作为起点
	GlobalStartLocation = GetActorLocation();

	//[4] 将 TargetLocation（相对坐标）转换为世界坐标作为终点
	GlobalTargetLocation = GetTransform().TransformPosition(TargetLocation);
}

//----------  

//<3> Tick — 每帧在服务器端推动平台沿目标方向移动
void AMovingPlatform::Tick(float DeltaTime)
{
	//[1] 调用父类 Tick  GameFramework/Actor.h
	Super::Tick(DeltaTime);

	//[2] 只有当前有触发器激活时平台才运动
	if (ActiveTriggers > 0)
	{
		//→[3] 仅在服务器端执行运动逻辑，移动结果会自动复制到客户端
		if (HasAuthority())
		{
			//[4] 获取当前世界位置
			FVector Location = GetActorLocation();

			//[5] 计算全程距离与已行进距离
			float JourneyLength   = (GlobalTargetLocation - GlobalStartLocation).Size();
			float JourneyTravelled = (Location - GlobalStartLocation).Size();

			//[6] 到达终点后交换起点和终点，实现往返
			if (JourneyTravelled >= JourneyLength)
			{
				FVector Swap         = GlobalStartLocation;
				GlobalStartLocation  = GlobalTargetLocation;
				GlobalTargetLocation = Swap;
			}

			//[7] 计算方向并按速度推动平台
			FVector Direction = (GlobalTargetLocation - GlobalStartLocation).GetSafeNormal();
			Location += Speed * DeltaTime * Direction;
			SetActorLocation(Location);
		}
	}
}

//----------  

//<4> AddActiveTrigger — 增加触发器激活计数（由 PlatformTrigger 调用）
void AMovingPlatform::AddActiveTrigger()
{
	//[1] 增加计数，平台开始（或继续）运动  PlatformTrigger.h
	ActiveTriggers++;
}

//----------  

//<5> RemoveActiveTrigger — 减少触发器激活计数（由 PlatformTrigger 调用）
void AMovingPlatform::RemoveActiveTrigger()
{
	//[1] 防止计数变为负数  PlatformTrigger.h
	if (ActiveTriggers > 0)
	{
		ActiveTriggers--;
	}
}
