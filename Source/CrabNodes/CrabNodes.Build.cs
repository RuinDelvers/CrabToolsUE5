// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CrabNodes : ModuleRules
{
	public CrabNodes(ReadOnlyTargetRules Target) : base(Target)
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
				"CrabToolsUE5",
                "CrabToolsUE5Editor",
                "Core",
				"UnrealEd",
				"DeveloperSettings",
				"GameplayTags",
				// ... add other public dependencies that you statically link with here ...
			});
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"Slate",
				"KismetCompiler",
				"EditorFramework",
				"UnrealEd",
                "GraphEditor",
				"SlateCore",
				"InputCore",
                "Kismet",
                "KismetWidgets",
				"ToolMenus",
				"AssetTools",
				"EditorSubsystem",
				"BlueprintGraph",
			});
	}
}
