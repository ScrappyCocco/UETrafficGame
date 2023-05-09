// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TrafficGame : ModuleRules
{
    public TrafficGame(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "UMG", "InputCore", "EnhancedInput", "LevelSequence", "MovieScene" });

        PrivateDependencyModuleNames.AddRange(new string[] { });

        OptimizeCode = CodeOptimization.InShippingBuildsOnly;
    }
}
