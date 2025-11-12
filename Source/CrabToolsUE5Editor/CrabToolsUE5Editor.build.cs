// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CrabToolsUE5Editor : ModuleRules
{
	public CrabToolsUE5Editor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			});
				
		
		PrivateIncludePaths.AddRange(
			new string[] {

			});
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"ApplicationCore",
                "CrabToolsUE5",
                "Core",
				"CoreUObject",
				"UnrealEd",
				"DeveloperSettings",
				"Blutility",
				"Slate",
				"SlateCore",
				"InputCore",
				"AssetRegistry",
				"Blutility",
				"UMG",
				// ... add other public dependencies that you statically link with here ...
			});
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CrabToolsUE5",
				"AssetDefinition",
				"Core",
				"CoreUObject",
				"Engine",
				"EditorFramework",
				"UnrealEd",
                "PropertyEditor",
				"ToolMenus",
				"AssetTools",
				"EditorSubsystem",
				"BlueprintGraph",
				"PlacementMode",
				"Blutility",
			});
	}
}
