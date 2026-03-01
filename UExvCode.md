# UExvCode.md — C++/UE5 编程错题本

---

## ❌ 错误 #1: SetReplicates 放在 HasAuthority() 判断内（错误次数: 1）

### 场景

继承 `AStaticMeshActor` 的 `AMovingPlatform`，希望平台可被所有客户端看见。

### ✗ 错误写法（UE4 遗留）

```cpp
// ✗ 错误: SetReplicates 放在 HasAuthority 内
//         客户端进入游戏时此函数已执行完毕，但客户端 HasAuthority()=false
//         导致客户端永远不会调用 SetReplicates(true)，平台对客户端不可见/不同步
void AMovingPlatform::BeginPlay()
{
    Super::BeginPlay();
    if (HasAuthority())          // ← 问题：客户端这里为 false
    {
        SetReplicates(true);     // ← 客户端永远执行不到这里！
        SetReplicateMovement(true);
    }
}
```

### ✓ 正确写法（UE5 推荐）

```cpp
// ✓ 正确: SetReplicates 在构造函数中调用
//          UE5 会在对象创建时统一处理复制注册，对服务器/客户端均生效
AMovingPlatform::AMovingPlatform()
{
    PrimaryActorTick.bCanEverTick = true;
    SetMobility(EComponentMobility::Movable);
    SetReplicates(true);          // ← 构造函数中设置，服务器和客户端均生效
}

// BeginPlay 中只需在服务器端开启移动同步
void AMovingPlatform::BeginPlay()
{
    Super::BeginPlay();
    if (HasAuthority())           // ← 仅服务器开启运动复制
    {
        SetReplicateMovement(true); // ← 这个放 HasAuthority 内是正确的
    }
    GlobalStartLocation  = GetActorLocation();
    GlobalTargetLocation = GetTransform().TransformPosition(TargetLocation);
}
```

### 📌 总结

| 函数                           | 推荐放置位置                       | 原因                   |
| ---------------------------- | ---------------------------- | -------------------- |
| `SetReplicates(true)`        | **构造函数**                     | 注册复制行为，必须在对象初始化时设置   |
| `SetReplicateMovement(true)` | `BeginPlay + HasAuthority()` | 仅服务器驱动物理运动，客户端接收数据即可 |

---

## ❌ 错误 #2: UE5 中仍使用旧式 BindAxis/HeadMountedDisplay（错误次数: 1）

### 场景

从 UE4 模板迁移 `APuzzlePlatformsCharacter`，Build.cs 中仍有 `HeadMountedDisplay` 模块。

### ✗ 错误写法（UE4）

```cpp
// ✗ 错误: UE5.7 中 HeadMountedDisplayFunctionLibrary 已废弃，无法编译
#include "HeadMountedDisplayFunctionLibrary.h"

// ✗ 错误: 旧式轴绑定，UE5 默认不启用 DefaultInput.ini 的旧 AxisMapping
void AMyChar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    PlayerInputComponent->BindAxis("MoveForward", this, &AMyChar::MoveForward);
    PlayerInputComponent->BindAxis("Turn", this, &AMyChar::AddControllerYawInput);
}
```

### ✓ 正确写法（UE5.7 Enhanced Input）

```cpp
// ✓ 正确: Build.cs 中改为 "EnhancedInput" 模块
// PublicDependencyModuleNames.AddRange(new string[] { ..., "EnhancedInput" });

// ✓ 头文件  
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"

// ✓ BeginPlay 中注册 InputMappingContext
void AMyChar::BeginPlay()
{
    Super::BeginPlay();
    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {
        if (auto* Sub = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            Sub->AddMappingContext(DefaultMappingContext, 0);
        }
    }
}

// ✓ 使用 UEnhancedInputComponent 绑定 InputAction
void AMyChar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    if (auto* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyChar::Move);
        EIC->BindAction(JumpAction, ETriggerEvent::Started,   this, &ACharacter::Jump);
    }
}
```

### 📌 总结

| UE4                                   | UE5.7                              |
| ------------------------------------- | ---------------------------------- |
| `HeadMountedDisplayFunctionLibrary.h` | ❌ 已废弃，移除                           |
| Build.cs `HeadMountedDisplay`         | 改为 `EnhancedInput`                 |
| `BindAxis("MoveForward", ...)`        | `EIC->BindAction(MoveAction, ...)` |
| `DefaultInput.ini AxisMappings`       | 改在编辑器中创建 `IMC` + `IA` 资产           |

---

## ❌ 错误 #3: UE5.7 中继续使用 UE4 旧路径 `OnlineSessionInterface.h`（错误次数: 1）

### 场景

项目从 UE4 升级到 UE5.7 后，`UPuzzlePlatformsGameInstance` 编译失败。

### ✖ 错误写法（UE4 旧路径）

```cpp
#include "OnlineSessionInterface.h"
```

### ✔ 正确写法（UE5.7）

```cpp
#include "Interfaces/OnlineSessionInterface.h"
```

### 结论

- `C1083` 在该场景下本质是头文件搜索路径变化。
- UE5.7 下 Online Session 接口应从 `Interfaces/` 目录包含。
- 本项目已修复并验证编辑器目标可编译通过。
