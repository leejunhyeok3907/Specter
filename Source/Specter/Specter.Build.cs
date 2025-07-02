// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Specter : ModuleRules {
	public Specter(ReadOnlyTargetRules Target) : base(Target) {
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicIncludePaths.AddRange(new string[] {
			"Specter",
			"Specter/Character",
			"Specter/AI",
            "Specter/Event",
            "Specter/UI",
			"Specter/WeaponComp"
			//"And so on..."
		});

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"HeadMountedDisplay",
			"UMG",
			"EnhancedInput",
			"NavigationSystem",
			"Navmesh",
			"Slate",
			"SlateCore",
			"AIModule",
			"GameplayTasks",
			"Niagara",
			"PhysicsCore",
			"JsonUtilities",
			"Json",
			"MediaAssets",
			
			"AICoverSystem"
		});
	}
}
