# 如何创建Steam多人连接游戏的新手教程（基于 PuzzlePlatforms / UE5.7）

## 0. 先看最终目标（你要做成什么）
你要实现的是这条链路：

1. 玩家A点 `Host` 创建会话（Session）
2. 玩家B点 `Join` 搜索并加入会话
3. 所有人进入 `Lobby`，人数够了后自动进 `Game`
4. 游戏中“会动的平台”等对象由服务器同步给所有客户端

一句话：**菜单发起会话 + GameInstance管联机 + 服务器权威同步**。

---

## 1. 先决条件（不做会直接失败）

### 1.1 项目必须启用模块与插件
- `PuzzlePlatforms.uproject`：启用 `OnlineSubsystemSteam`
- `Source/PuzzlePlatforms/PuzzlePlatforms.Build.cs`：包含
  - `OnlineSubsystem`
  - `OnlineSubsystemUtils`
  - 动态加载 `OnlineSubsystemSteam`

### 1.2 GameInstance必须设置成你的联机类
在 `Config/DefaultEngine.ini` 中确认：

```ini
[/Script/EngineSettings.GameMapsSettings]
GameInstanceClass=/Script/PuzzlePlatforms.PuzzlePlatformsGameInstance
```

不设置的后果：菜单能点，但 `Host/Join` 逻辑根本不会走到你写的 C++。

### 1.3 Steam配置要分“本地调试”和“真Steam联机”
当前项目里是：

```ini
[OnlineSubsystem]
DefaultPlatformService=Steam

[OnlineSubsystemSteam]
bEnabled=False
SteamDevAppId=480
```

- `bEnabled=False`：更适合本机双开调试（常配合 `-nosteam`）
- `bEnabled=True`：真实 Steam 联机测试（建议两账号两机器）

**重点**：很多新手以为“写完代码就能Steam联机”，结果卡在这里。

---

## 2. 最小可用流程（照着做就能通）

## 2.1 在 GameInstance 里接管 Session
核心文件：
- `Source/PuzzlePlatforms/PuzzlePlatformsGameInstance.h`
- `Source/PuzzlePlatforms/PuzzlePlatformsGameInstance.cpp`

你要做4件事：

1. `Init()` 里绑定回调
- `OnCreateSessionComplete`
- `OnDestroySessionComplete`
- `OnFindSessionsComplete`
- `OnJoinSessionComplete`

2. `Host(ServerName)`
- 有旧会话先销毁
- 没旧会话直接 `CreateSession()`

3. `CreateSession()` 设置会话参数
- `bShouldAdvertise = true`
- `bUsesPresence = true`
- `bUseLobbiesIfAvailable = true`
- 自定义房间名 `ServerName`

4. 创建成功后 `ServerTravel("...Lobby?listen")`
- `?listen` 代表“本机作为可连接主机”

## 2.2 在菜单里只做“发命令”
核心文件：
- `Source/PuzzlePlatforms/MenuSystem/MainMenu.cpp`
- `Source/PuzzlePlatforms/MenuSystem/MenuInterface.h`

UI按钮不直接碰底层 Session，只调用接口：
- `HostServer()` -> `MenuInterface->Host(ServerName)`
- `OpenJoinMenu()` -> `MenuInterface->RefreshServerList()`
- `JoinServer()` -> `MenuInterface->Join(SelectedIndex)`

这样分层简单：**UI只负责点，GameInstance负责联机**。

## 2.3 Lobby人数够了自动开局
核心文件：
- `Source/PuzzlePlatforms/LobbyGameMode.cpp`

流程：
- `PostLogin` 计数玩家
- 达到阈值后开定时器
- `StartGame()` 里
  - `GameInstance->StartSession()`
  - `bUseSeamlessTravel = true`
  - `ServerTravel("...Game?listen")`

---

## 3. 网络同步重点（新手最容易懵的一节）

先记住一句：

> **服务器算结果，客户端看结果。**

在本项目的移动平台里（`MovingPlatform.cpp`）做法很标准：

1. 构造函数 `SetReplicates(true)`
- 表示这个Actor参与网络复制

2. `BeginPlay` 里仅服务器 `SetReplicateMovement(true)`
- 由服务器推送位置变化给客户端

3. `Tick` 里用 `HasAuthority()` 限制
- 只有服务器执行移动逻辑 `SetActorLocation`
- 客户端不自己算，避免漂移/打架

### 为什么要这么做？
如果客户端也自己动平台，会出现：
- A客户端看到平台在A点
- B客户端看到平台在B点
- 人站上平台会抖动、穿模、瞬移

这就是“非权威同步”常见灾难。

---

## 4. 叉勾对照（直接背这张表，能避开80%坑）

| 场景 | ❌ 错误写法 | ✅ 正确写法 |
|---|---|---|
| Actor复制开关 | 在 `BeginPlay + HasAuthority` 里才 `SetReplicates(true)` | 在**构造函数**里 `SetReplicates(true)` |
| 移动逻辑 | 客户端和服务器都在 `Tick` 里移动 | 仅服务器 `HasAuthority()` 内移动，客户端只接收 |
| 开房后切图 | `ServerTravel("/Map/Lobby")` | `ServerTravel("/Map/Lobby?listen")` |
| 搜房间 | `FindSessions` 但没设置 Lobby/Presence 查询 | 搜索时设置 `SEARCH_LOBBIES=true`，并与创建参数一致 |
| 按钮直连底层 | UI里直接调用 SessionInterface | UI -> `IMenuInterface` -> GameInstance |
| Steam调试 | 一上来就本机双开+Steam启用 | 先 `bEnabled=False` 跑通流程，再两机两号测Steam |

---

## 5. 零基础语法速记（只看会话代码需要的）

- `if (!ensure(X)) return;`
  - 含义：运行时检查，失败就提前退出，防崩溃

- `IOnlineSessionPtr`
  - 含义：Online Session接口智能指针，拿它调 `CreateSession/JoinSession`

- `TSharedPtr<FOnlineSessionSearch>`
  - 含义：会话搜索对象，存储 `FindSessions` 的结果

- `AddUObject(this, &Class::Func)`
  - 含义：把“异步完成回调”绑定到当前对象成员函数

- `ClientTravel(Address, TRAVEL_Absolute)`
  - 含义：客户端跳转到目标服务器地址

---

## 6. 手把手测试清单（按顺序点）

1. 编译 `PuzzlePlatformsEditor Win64 Development`
2. 运行两个独立进程（本机双开调试）
3. A窗口点 Host，输入房间名
4. B窗口点 Join，刷新列表，选中后 Confirm Join
5. 两人进入 Lobby
6. 达到人数后自动跳 Game
7. 观察移动平台：两端位置应一致且平滑

日志建议关注：
- `Found subsystem ...`
- `Starting Find Session`
- `Finished Find Session`
- `Joining ...`

---

## 7. 常见问题快速定位

### Q1：看不到房间
- 检查是否调用 `RefreshServerList()`
- 检查 `SEARCH_LOBBIES` 与 `bUseLobbiesIfAvailable/bUsesPresence` 是否匹配
- 检查 Steam 是否冲突（本机双开常见）

### Q2：能进房但平台不同步
- 检查 `AMovingPlatform` 是否 `SetReplicates(true)`
- 检查移动逻辑是否只在服务器运行
- 检查是否启用了 `SetReplicateMovement(true)`

### Q3：Host后别人连不上
- 检查 `ServerTravel` 是否带 `?listen`
- 检查 `NetDriverDefinitions` 是否配置到 SteamNetDriver

---

## 8. 一页总结（记住这5条）

1. **GameInstance是联机中枢**：创建/搜索/加入会话都放这里。
2. **UI不直接碰Session**：通过 `IMenuInterface` 转发。
3. **服务器权威**：移动/状态计算只在服务器做。
4. **复制开关位置要对**：`SetReplicates(true)` 放构造函数。
5. **先本地跑通，再上Steam真测**：调试效率最高。

如果你按本文顺序做完，已经具备从0搭建 Steam 多人连接游戏的完整骨架了。下一步只是在这个骨架上加玩法。
