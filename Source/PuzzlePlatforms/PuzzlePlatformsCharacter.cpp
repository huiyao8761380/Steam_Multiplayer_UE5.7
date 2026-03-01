//@004 四. 第三人称玩家角色实现
//     UE5.7 变更:
//       - 移除 HeadMountedDisplayFunctionLibrary.h (已废弃)
//       - 移除 OnResetVR / TurnAtRate / LookUpAtRate / TouchStarted / TouchStopped
//       - SetupPlayerInputComponent 改为 Enhanced Input 方式
//       - BeginPlay 中注册 DefaultMappingContext 到 EnhancedInputLocalPlayerSubsystem

#include "PuzzlePlatformsCharacter.h"
// UE5 Enhanced Input  EnhancedInputSubsystems.h
#include "EnhancedInputSubsystems.h"
// UE5 Enhanced Input 组件  EnhancedInputComponent.h
#include "EnhancedInputComponent.h"
// Input Action 值类型  InputActionValue.h
#include "InputActionValue.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

//----------  

//<1> 构造函数 — 初始化胶囊体、相机组件和移动参数
APuzzlePlatformsCharacter::APuzzlePlatformsCharacter()
{
	// [1] 设置胶囊体碰撞大小
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// [2] 角色不随控制器旋转，相机独立旋转
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw   = false;
	bUseControllerRotationRoll  = false;

	// [3] 移动组件设置: 朝移动方向旋转角色
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// [4] 创建弹簧臂组件，跟随控制器旋转（相机绕角色旋转）  GameFramework/SpringArmComponent.h
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;	// 跟随控制器旋转弹簧臂

	// [5] 创建跟随相机，不随控制器旋转（弹簧臂已处理旋转）  Camera/CameraComponent.h
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// [6] Enhanced Input 资产引用默认为空，需在蓝图细节面板赋值
}

//----------  

//<2> BeginPlay — 向本地玩家子系统注册输入映射上下文
void APuzzlePlatformsCharacter::BeginPlay()
{
	// [1] 调用父类 BeginPlay  GameFramework/Character.h
	Super::BeginPlay();

	// [2] 获取本地玩家的 Enhanced Input 子系统，将 DefaultMappingContext 注册进去
	//     优先级 0 表示无覆盖关系的默认上下文  EnhancedInputSubsystems.h
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

//----------  

//<3> Move — 处理移动输入 (IA_Move, Axis2D: X=前后, Y=左右)
void APuzzlePlatformsCharacter::Move(const FInputActionValue& Value)
{
	// [1] 从 FInputActionValue 中取出 Axis2D 值
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// [2] 获取控制器朝向的 Yaw，得到前向/右向单位向量
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// [3] 前向方向（X轴）
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		// [4] 右向方向（Y轴）
		const FVector RightDirection   = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// [5] 添加移动输入
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection,   MovementVector.X);
	}
}

//----------  

//<4> Look — 处理视角输入 (IA_Look, Axis2D: X=左右偏航, Y=上下俯仰)
void APuzzlePlatformsCharacter::Look(const FInputActionValue& Value)
{
	// [1] 从 FInputActionValue 中取出 Axis2D 值
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// [2] X 轴对应偏航（Yaw），Y 轴对应俯仰（Pitch）
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

//----------  

//<5> SetupPlayerInputComponent — 绑定 Enhanced Input 动作到回调函数
void APuzzlePlatformsCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// [1] 将 UInputComponent 转型为 UEnhancedInputComponent 以使用新 API
	//     Build.cs 中须包含 "EnhancedInput" 模块  EnhancedInputComponent.h
	if (UEnhancedInputComponent* EnhancedInputComponent =
		Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// [2] 跳跃: Started 时调用 Jump(), Completed 时调用 StopJumping()
		//          Jump/StopJumping 继承自 ACharacter  GameFramework/Character.h
		EnhancedInputComponent->BindAction(JumpAction,   ETriggerEvent::Started,   this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction,   ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// [3] 移动: Triggered 时持续调用 Move()
		EnhancedInputComponent->BindAction(MoveAction,   ETriggerEvent::Triggered, this, &APuzzlePlatformsCharacter::Move);

		// [4] 视角: Triggered 时持续调用 Look()
		EnhancedInputComponent->BindAction(LookAction,   ETriggerEvent::Triggered, this, &APuzzlePlatformsCharacter::Look);
	}
}
