// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class LuaGenerator : ModuleRules
{
	public LuaGenerator(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
                "LuaGenerator/Public",
                "Programs/UnrealHeaderTool/Public",
            });
				
		PrivateIncludePaths.AddRange(
			new string[] {
                "Developer/LuaGenerator/Private",
            });
			
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "CoreUObject",
                "Json",
            });
			
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "Projects",
            });
	}
}
