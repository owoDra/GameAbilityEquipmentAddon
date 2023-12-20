// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GAEAddon : ModuleRules
{
	public GAEAddon(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[]
            {
                ModuleDirectory,
                ModuleDirectory + "/GAEAddon",
            }
        );


        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "ModularGameplay",
                "GameplayTags",
                "GameplayAbilities",
                "NetCore",
            }
        );


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "GFCore",
                "GAExt",
            }
        );

        SetupIrisSupport(Target);
    }
}
