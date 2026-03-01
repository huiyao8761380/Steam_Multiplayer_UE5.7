// UE5.7 目标配置:
// 功能: 定义 PuzzlePlatforms 游戏目标（Game Target）构建规则

using UnrealBuildTool;
using System.Collections.Generic;

public class PuzzlePlatformsTarget : TargetRules
{
	public PuzzlePlatformsTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V6;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("PuzzlePlatforms");
	}
}
