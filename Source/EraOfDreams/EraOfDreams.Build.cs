// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class EraOfDreams : ModuleRules
{
	public EraOfDreams(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] 
		{ 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput", 
			"LevelSequence", 
			"MovieScene",
			"MotionWarping",
            "GameplayTags",
            "AnimGraphRuntime",
            "AnimationWarpingRuntime",
            "Chooser",
            "PoseSearch",
            "StructUtils"
        });

        PublicIncludePaths.Add("EraOfDreams/Public");
        PublicIncludePaths.Add("EraOfDreams/Public/Character");
        PublicIncludePaths.Add("EraOfDreams/Public/Character/Sera");
        PublicIncludePaths.Add("EraOfDreams/Public/Concert");
        PublicIncludePaths.Add("EraOfDreams/Public/Input");
        PublicIncludePaths.Add("EraOfDreams/Public/MoveParkoru/Core");
        PublicIncludePaths.Add("EraOfDreams/Public/MoveParkoru/Traversal");
        PublicIncludePaths.Add("EraOfDreams/Public/MoveParkoru/Samples");
    }
}
