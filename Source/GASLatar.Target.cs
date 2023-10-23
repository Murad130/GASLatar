// Copyright Latar

using UnrealBuildTool;
using System.Collections.Generic;

public class GASLatarTarget : TargetRules
{
	public GASLatarTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V4;

		ExtraModuleNames.AddRange( new string[] { "GASLatar" } );
	}
}
