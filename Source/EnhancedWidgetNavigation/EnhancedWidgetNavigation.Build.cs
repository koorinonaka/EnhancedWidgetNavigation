// Copyright 2022 koorinonaka, All Rights Reserved.

using UnrealBuildTool;

public class EnhancedWidgetNavigation : ModuleRules
{
	public EnhancedWidgetNavigation(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PrivatePCHHeaderFile = "Private/EnhancedWidgetNavigationPCH.h";

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"CoreUObject",
			"Engine",
			"Slate",
			"SlateCore",
			"UMG",
			"InputCore",
			"EnhancedInput",
			"DeveloperSettings",
		});

		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(new string[]
			{
				"MessageLog",
			});
		}
	}
}
