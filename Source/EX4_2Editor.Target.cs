

using UnrealBuildTool;
using System.Collections.Generic;

public class EX4_2EditorTarget : TargetRules
{
	public EX4_2EditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_5;
		ExtraModuleNames.Add("CX");
	}
}
