// Copyright Latar

using UnrealBuildTool;
using System.Collections.Generic;

public class GASLatarEditorTarget : TargetRules
{
	public GASLatarEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V4;

		ExtraModuleNames.AddRange( new string[] { "GASLatar" } );

        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

    }
}
