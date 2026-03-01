# UEeditor.md — UE5.7 编辑器操作说明文档

---

## 0. 联机调试上下文（Steam/NULL 与双开 Host/Join）

说明: `bEnabled=False` 能本机双开并互相 Join，`bEnabled=True` 时本机双开通常无法正常 Join，这是子系统机制差异导致的。  
注意事项: `NULL` 子系统用于本地/LAN 调试；`Steam` 子系统用于在线会话，通常需要两个 Steam 账号（建议两台机器）进行真实联机测试。

**原因结论:**

1. 当前代码中会根据子系统设置 `bIsLANMatch`：当子系统为 `NULL` 时走 LAN 匹配，因此一台电脑两个进程容易互相发现并 Join。
2. 当启用 Steam（`[OnlineSubsystemSteam] bEnabled=True`）后，走的是 Steam Lobby/Presence，不再是 LAN 广播。
3. Steam 模式下同一台电脑通常只有一个有效 Steam 登录身份，第二个端容易出现 Steam 初始化或身份冲突问题，导致无法识别或无法 Join。

**编辑器内双开并手动 Host/Join（等效 Start2Game.bat 调试）:**

1. 打开编辑器 `Play` 设置。
2. `Play Mode` 选择 `Standalone Game`。
3. `Number of Players` 设置为 `2`。
4. 关闭 `Run Under One Process`（使用两个独立进程）。
5. `Net Mode` 选择 `Play Standalone`（手动在 UI 中 Host/Join）。
6. 点击 Play 后，在两个窗口中分别执行: 一个窗口点击 Host，另一个窗口刷新并 Join。
7. 若要稳定本机双开调试，在 `Additional Launch Parameters` 添加 `-nosteam`（或将 Steam 暂时禁用）。

---

## ※ 1. PuzzlePlatformsCharacter.h

说明: Enhanced Input 需要在 UE5 编辑器中创建 IMC/IA 输入资产，并赋值给角色蓝图  
注意事项: 未赋值则角色无法移动/跳跃/旋转视角，DefaultMappingContext/MoveAction/LookAction/JumpAction 均须赋值

**UE 操作步骤:**

1. 内容浏览器 → 右键 → **Input → Input Mapping Context** → 命名为 `IMC_Default`
2. 内容浏览器 → 右键 → **Input → Input Action** → 创建以下三个资产:
   - `IA_Jump` (Value Type: **Digital/Bool**)
   - `IA_Move` (Value Type: **Axis2D/Vector2D**)
   - `IA_Look` (Value Type: **Axis2D/Vector2D**)
3. 打开 `IMC_Default`，添加上述 IA 并绑定按键:
   - `IA_Jump` → **Space Bar**
   - `IA_Move` → **W/A/S/D**（W=+Y, S=-Y, D=+X, A=-X），建议使用 **WASD 2D Composite**
   - `IA_Look` → **Mouse XY 2D Axis**
4. 打开继承自 `APuzzlePlatformsCharacter` 的**角色蓝图**（如 `BP_ThirdPersonCharacter`），在**细节面板 → Input** 区域:
   - `Default Mapping Context` → 赋值 `IMC_Default`
   - `Jump Action` → 赋值 `IA_Jump`
   - `Move Action` → 赋值 `IA_Move`
   - `Look Action` → 赋值 `IA_Look`
5. 编译并保存蓝图

---

## ※ 2. PlatformTrigger.h

说明: APlatformTrigger 继承自 AActor，需要在编辑器中手动配置触发平台列表  
注意事项: `PlatformsToTrigger` 数组在细节面板中拖入对应的 `AMovingPlatform` Actor 引用即可

**UE 操作步骤:**

1. 将 `BP_PlatformTrigger`（继承 `APlatformTrigger`）放置在场景中
2. 选中 `BP_PlatformTrigger`，在**细节面板 → Platforms To Trigger** 中点击 `+`
3. 将场景中对应的 `BP_MovingPlatform` Actor 拖入数组元素槽
4. 设置 BoxComponent 的碰撞大小（细节面板 → Shape → Box Extent）

---

## ※ 3. PuzzlePlatformsGameInstance.h

说明: 需要在项目设置中将 `UPuzzlePlatformsGameInstance` 设为默认 GameInstance  
注意事项: 不设置则 Session 功能无法工作；Steam 测试需开启 `OnlineSubsystemSteam` 插件

**UE 操作步骤:**

1. **编辑 → 项目设置 → 地图和模式 → Game Instance Class** → 选择 `PuzzlePlatformsGameInstance`

2. **编辑 → 插件** → 搜索 `Online Subsystem Steam` → 勾选启用 → 重启编辑器

3. `Config/DefaultEngine.ini` 确认以下内容存在:
   
   ```ini
   [OnlineSubsystem]
   DefaultPlatformService=Steam
   
   [/Script/Engine.GameEngine]
   +NetDriverDefinitions=(DefName="GameNetDriver",DriverClassName="OnlineSubsystemSteam.SteamNetDriver",DriverClassNameFallback="OnlineSubsystemUtils.IpNetDriver")
   ```

---

## 4. PuzzlePlatformsGameInstance.h（UE5.7 头文件迁移）

说明: UE4 到 UE5.7 迁移后，`OnlineSessionInterface.h` 需要改为 `Interfaces/OnlineSessionInterface.h`，否则会出现 C1083。
注意事项: 若 `Build.cs` 未包含 `OnlineSubsystem` 与 `OnlineSubsystemUtils`，同样会导致 Online Session 相关类型无法解析。

**UE 操作步骤:**

1. 在 VS 打开 `Source/PuzzlePlatforms/PuzzlePlatformsGameInstance.h`。
2. 将 `#include "OnlineSessionInterface.h"` 改为 `#include "Interfaces/OnlineSessionInterface.h"`。
3. 重新编译 `PuzzlePlatformsEditor Win64 Development`。
4. 若仍报 Online 相关错误，检查 `PuzzlePlatforms.Build.cs` 依赖模块配置。

---

## ※ 5. MovingPlatform.h

说明: `AMovingPlatform` 依赖关卡中摆放的可移动静态网格 Actor，并通过 `TargetLocation` 控制往返终点。  
注意事项: 平台移动逻辑只在服务器端执行；若是联机测试，需确认平台 Actor 勾选可复制相关选项。

**UE 操作步骤:**

1. 在内容浏览器创建或打开 `BP_MovingPlatform`（父类 `AMovingPlatform`）。
2. 将 `BP_MovingPlatform` 放入关卡。
3. 选中该 Actor，在细节面板设置:
   - `Speed`（建议先用 20~80 便于观察）
   - `TargetLocation`（视口中会出现可拖拽控件，拖到目标点）
4. 若用于多人联机验证，在细节面板确认该 Actor 的网络复制选项已启用（以 C++ 配置为准）。
5. 运行后观察平台是否在起点与目标点之间往返移动。

---

## ※ 6. PuzzlePlatformsGameMode.h

说明: 基础 GameMode 会指定默认 Pawn 为 `ThirdPersonCharacter` 蓝图。  
注意事项: 若关卡 World Settings 覆盖了 GameMode，项目默认设置不会生效。

**UE 操作步骤:**

1. 进入 **编辑 → 项目设置 → 地图和模式**。
2. 将 `Default GameMode` 设为 `PuzzlePlatformsGameMode`（或你的派生类）。
3. 打开关卡后检查 **World Settings → GameMode Override**，避免被其他模式覆盖。
4. 运行游戏确认出生 Pawn 为第三人称角色蓝图。

---

## ※ 7. LobbyGameMode.h

说明: 大厅模式会在玩家数量达到阈值后启动倒计时，再切换到游戏地图。  
注意事项: 该逻辑在服务器端触发，需在多人模式下验证 `PostLogin/Logout` 计数。

**UE 操作步骤:**

1. 打开大厅地图（例如 `Lobby`）。
2. 在 **World Settings** 将 `GameMode Override` 设为 `LobbyGameMode`。
3. 在 `Project Settings → Maps & Modes` 检查 `Transition Map` 是否已设置（联机切图更稳定）。
4. 使用 Standalone 双开或多机测试，让第二名玩家进入大厅。
5. 观察倒计时后是否自动 `ServerTravel` 到游戏地图。

---

## ※ 8. MainMenu.h / ServerRow.h

说明: 主菜单与服务器行依赖 UMG 的 `BindWidget` 绑定，控件命名必须与 C++ 成员名一致。  
注意事项: 任何控件改名都会导致 `Initialize` 绑定失败（`ensure` 报错）。

**UE 操作步骤:**

1. 打开 `WBP_MainMenu`，确认以下控件名称与 C++ 完全一致:
   - `HostButton` `JoinButton` `QuitButton`
   - `CancelJoinMenuButton` `ConfirmJoinMenuButton`
   - `CancelHostMenuButton` `ConfirmHostMenuButton`
   - `MenuSwitcher` `MainMenu` `JoinMenu` `HostMenu`
   - `ServerHostName` `ServerList`
2. 打开 `WBP_ServerRow`，确认控件:
   - `RowButton` `ServerName` `HostUser` `ConnectionFraction`
3. 在 `WBP_MainMenu` 的列表创建流程中确保 `ServerRowClass` 指向 `WBP_ServerRow`。
4. 运行后点击 Join 菜单，检查服务器列表能正确刷新与选中。

---

## ※ 9. InGameMenu.h / MenuWidget.h

说明: 游戏内暂停菜单基于 `UMenuWidget::Setup/Teardown` 切换输入模式（UIOnly 与 GameOnly）。  
注意事项: 若菜单弹出后无法操作角色，通常是 `Teardown` 未执行或按钮绑定缺失。

**UE 操作步骤:**

1. 打开 `WBP_InGameMenu`，确认绑定按钮名称:
   - `CancelButton`
   - `QuitButton`
2. 在角色或控制器中绑定打开暂停菜单的输入事件（调用 `InGameLoadMenu`）。
3. 运行后打开菜单，验证:
   - 鼠标显示且可操作 UI
   - 点击 `Cancel` 后返回游戏输入
   - 点击 `Quit` 后执行返回主菜单逻辑
