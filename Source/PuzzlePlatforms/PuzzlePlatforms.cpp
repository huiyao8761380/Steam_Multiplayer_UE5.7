//@001 一. 游戏模块主实现
//     功能: 注册 PuzzlePlatforms 主游戏模块（UE5 启动入口）

#include "PuzzlePlatforms.h"
#include "Modules/ModuleManager.h"

//----------  

//<1> IMPLEMENT_PRIMARY_GAME_MODULE — 注册 PuzzlePlatforms 为游戏主模块
IMPLEMENT_PRIMARY_GAME_MODULE(FDefaultGameModuleImpl, PuzzlePlatforms, "PuzzlePlatforms");
