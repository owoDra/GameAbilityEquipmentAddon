// Copyright (C) 2023 owoDra

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
                "GFCore",
            }
        );


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "GAExt",
            }
        );

        SetupIrisSupport(Target);
    }
}
