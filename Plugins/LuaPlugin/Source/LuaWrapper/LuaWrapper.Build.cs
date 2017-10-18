// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class LuaWrapper : ModuleRules
{
    public LuaWrapper(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
                "LuaWrapper/Public",
            });
				
		PrivateIncludePaths.AddRange(
			new string[] {
                "LuaWrapper/Private",
            });
			
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "Engine",
                "CoreUObject",
                "Slate",
                "SlateCore",
                "InputCore",
                "MyProject",
                "UMG",
            });
    }
}
