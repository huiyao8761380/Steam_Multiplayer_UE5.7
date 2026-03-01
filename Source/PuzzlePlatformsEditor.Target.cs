// UE5.7 目标配置:
// 功能: 定义 PuzzlePlatforms 编辑器目标（Editor Target）构建规则

using UnrealBuildTool;
using System.Collections.Generic;

public class PuzzlePlatformsEditorTarget : TargetRules
{
	public PuzzlePlatformsEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V6;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("PuzzlePlatforms");
	}
}
