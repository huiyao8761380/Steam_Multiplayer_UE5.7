# UEcSteps.md — C++ 源码文件索引与迁移校对（UE5.7）

---

## 文件索引A — 头文件（`.h`）

| 编号 | 文件 | 功能描述 |
|------|------|----------|
| 001 一 | [PuzzlePlatforms.h](Source/PuzzlePlatforms/PuzzlePlatforms.h) | 游戏模块主头文件，提供模块最小依赖 |
| 002 二 | [MovingPlatform.h](Source/PuzzlePlatforms/MovingPlatform.h) | 可移动平台 Actor，服务器权威往返移动并支持触发器激活 |
| 003 三 | [PlatformTrigger.h](Source/PuzzlePlatforms/PlatformTrigger.h) | 平台触发器 Actor，处理 Overlap 并通知平台增减激活计数 |
| 004 四 | [PuzzlePlatformsCharacter.h](Source/PuzzlePlatforms/PuzzlePlatformsCharacter.h) | 第三人称角色，基于 UE5 Enhanced Input 处理移动/视角/跳跃 |
| 005 五 | [MenuInterface.h](Source/PuzzlePlatforms/MenuSystem/MenuInterface.h) | 菜单业务接口定义（Host/Join/Refresh/LoadMainMenu） |
| 006 六 | [MenuWidget.h](Source/PuzzlePlatforms/MenuSystem/MenuWidget.h) | 菜单基类，统一 Setup/Teardown 与输入模式切换 |
| 007 七 | [MainMenu.h](Source/PuzzlePlatforms/MenuSystem/MainMenu.h) | 主菜单 Widget，负责 Host/Join 与服务器列表交互 |
| 008 八 | [InGameMenu.h](Source/PuzzlePlatforms/MenuSystem/InGameMenu.h) | 游戏内暂停菜单 Widget，处理取消与退出 |
| 009 九 | [ServerRow.h](Source/PuzzlePlatforms/MenuSystem/ServerRow.h) | 服务器列表单行 Widget，展示并回传选中状态 |
| 010 十 | [PuzzlePlatformsGameMode.h](Source/PuzzlePlatforms/PuzzlePlatformsGameMode.h) | 基础 GameMode，设置默认 Pawn |
| 011 十一 | [LobbyGameMode.h](Source/PuzzlePlatforms/LobbyGameMode.h) | 大厅 GameMode，玩家计数达标后倒计时开局 |
| 012 十二 | [PuzzlePlatformsGameInstance.h](Source/PuzzlePlatforms/PuzzlePlatformsGameInstance.h) | 核心 GameInstance，管理 Session 创建/查找/加入/销毁 |

---

## 文件索引B — 源文件（`.cpp`）

| 对应编号 | 文件 | 功能描述 |
|---------|------|----------|
| 001 一 | [PuzzlePlatforms.cpp](Source/PuzzlePlatforms/PuzzlePlatforms.cpp) | 注册主游戏模块 `IMPLEMENT_PRIMARY_GAME_MODULE` |
| 002 二 | [MovingPlatform.cpp](Source/PuzzlePlatforms/MovingPlatform.cpp) | 平台移动实现与服务器端复制控制 |
| 003 三 | [PlatformTrigger.cpp](Source/PuzzlePlatforms/PlatformTrigger.cpp) | Overlap 回调实现，驱动平台激活计数 |
| 004 四 | [PuzzlePlatformsCharacter.cpp](Source/PuzzlePlatforms/PuzzlePlatformsCharacter.cpp) | Enhanced Input 注册与移动/视角输入实现 |
| 005 五 | [MenuInterface.cpp](Source/PuzzlePlatforms/MenuSystem/MenuInterface.cpp) | 接口实现单元（当前为空实现） |
| 006 六 | [MenuWidget.cpp](Source/PuzzlePlatforms/MenuSystem/MenuWidget.cpp) | 菜单显示/隐藏与输入模式切换实现 |
| 007 七 | [MainMenu.cpp](Source/PuzzlePlatforms/MenuSystem/MainMenu.cpp) | 主菜单按钮绑定、服务器列表刷新与 Join 触发 |
| 008 八 | [InGameMenu.cpp](Source/PuzzlePlatforms/MenuSystem/InGameMenu.cpp) | 暂停菜单按钮绑定与返回逻辑 |
| 009 九 | [ServerRow.cpp](Source/PuzzlePlatforms/MenuSystem/ServerRow.cpp) | 服务器行初始化与点击回调 |
| 010 十 | [PuzzlePlatformsGameMode.cpp](Source/PuzzlePlatforms/PuzzlePlatformsGameMode.cpp) | 默认 Pawn 蓝图查找并赋值 |
| 011 十一 | [LobbyGameMode.cpp](Source/PuzzlePlatforms/LobbyGameMode.cpp) | 大厅玩家计数、倒计时、无缝切图逻辑 |
| 012 十二 | [PuzzlePlatformsGameInstance.cpp](Source/PuzzlePlatforms/PuzzlePlatformsGameInstance.cpp) | Session 全流程与菜单驱动实现 |

---

## UE5 迁移与配置校对

1. 引擎版本（来自 `.uproject`）: `EngineAssociation = 5.7`。
2. 模块依赖（来自 `PuzzlePlatforms.Build.cs`）:
   - `Core` `CoreUObject` `Engine` `InputCore` `EnhancedInput` `UMG` `OnlineSubsystem` `OnlineSubsystemUtils`
3. 动态加载模块:
   - `OnlineSubsystemSteam`
4. 启用插件（来自 `.uproject`）:
   - `OnlineSubsystemSteam`
   - `LanguageOne`

---

## 迁移检查结论（2026-03-01）

1. 已按 UE5 语义统一 `Source/PuzzlePlatforms` 注释风格，清理 UE4 模板残留英文注释。
2. Session 相关头文件路径已使用 UE5 形式:
   - `#include "Interfaces/OnlineSessionInterface.h"`
3. 联机调试建议:
   - 本机双开验证玩法优先 `-nosteam / NULL`。
   - 真实 Steam 联机建议双账号多机测试。
