

using UnrealBuildTool;
using System.Collections.Generic;

public class EX4_2Target : TargetRules
{
	public EX4_2Target(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_5;
		ExtraModuleNames.Add("CX");
	}
}
