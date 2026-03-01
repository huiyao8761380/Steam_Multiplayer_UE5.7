//@004 四. 第三人称玩家角色
//     功能: 处理玩家移动/视角/跳跃输入，管理弹簧臂相机
//     UE5.7 变更: 使用 Enhanced Input 系统替代旧式 BindAxis 轴映射
//     移除: HeadMountedDisplay/OnResetVR/TurnAtRate/LookUpAtRate (UE4 遗留)

//※ 1. 虚幻5操作流程：
//说明: Enhanced Input 需要在 UE 编辑器中创建资产并赋值给 Character 蓝图 DefaultPawn
//注意事项: 未赋值则角色无法移动/跳跃/旋转视角，DefaultMappingContext/MoveAction/LookAction/JumpAction 均须赋值
//UE 操作步骤:
//   1. 内容浏览器 → 右键 → Input → Input Mapping Context → 命名为 IMC_Default
//   2. 内容浏览器 → 右键 → Input → Input Action → 分别创建:
//      IA_Move (Axis2D), IA_Look (Axis2D), IA_Jump (Digital)
//   3. 打开 IMC_Default, 添加上述三个 IA, 为 IA_Move 绑定 WASD, IA_Look 绑定鼠标XY, IA_Jump 绑定空格
//   4. 打开 ThirdPersonCharacter 蓝图 (继承本类), 在细节面板找到
//      DefaultMappingContext / MoveAction / LookAction / JumpAction, 分别赋值上述资产

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
// UE5 Enhanced Input 头文件  EnhancedInput/Public/InputMappingContext.h
#include "InputMappingContext.h"
#include "InputAction.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PuzzlePlatformsCharacter.generated.h"

struct FInputActionValue;

UCLASS(config=Game)
class APuzzlePlatformsCharacter : public ACharacter
{
	GENERATED_BODY()

	/** 弹簧臂，将相机固定在角色身后  GameFramework/SpringArmComponent.h */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;		//(1) 弹簧臂组件

	/** 跟随相机  Camera/CameraComponent.h */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;		//(2) 跟随相机组件

public:
	APuzzlePlatformsCharacter();	//(3) 构造函数，初始化相机与移动参数

	// -------- Enhanced Input 资产引用（在蓝图细节面板赋值） --------

	/** 默认输入映射上下文 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* DefaultMappingContext;	//(4) IMC — 绑定输入映射表

	/** 跳跃输入动作 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* JumpAction;		//(5) IA_Jump — 跳跃动作

	/** 移动输入动作 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* MoveAction;		//(6) IA_Move — 前后左右移动动作

	/** 视角输入动作 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* LookAction;		//(7) IA_Look — 鼠标/摇杆视角旋转动作

protected:
	// -------- Enhanced Input 回调函数 --------

	void Move(const FInputActionValue& Value);	//(8) 处理移动输入（Axis2D）
	void Look(const FInputActionValue& Value);	//(9) 处理视角输入（Axis2D）

protected:
	// APawn 接口
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 游戏开始时注册 IMC
	virtual void BeginPlay() override;

public:
	/** 返回 CameraBoom 子对象 */
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** 返回 FollowCamera 子对象 */
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
