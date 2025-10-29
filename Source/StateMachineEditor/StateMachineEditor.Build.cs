// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StateMachineEditor : ModuleRules
{
	public StateMachineEditor(ReadOnlyTargetRules Target) : base(Target)
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
				"Core",
				"UnrealEd",
				"DeveloperSettings",
				// ... add other public dependencies that you statically link with here ...
			});
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"AssetDefinition",
				"Core",
				"CoreUObject",
				"Engine",
				"KismetCompiler",
				"EditorFramework",
				"UnrealEd",
                "GraphEditor",
				"Slate",
				"SlateCore",
				"InputCore",
                "Kismet",
                "KismetWidgets",
                "PropertyEditor",
				"ToolMenus",
				"AssetTools",
				"EditorSubsystem",
				"BlueprintGraph",
				"Projects",
			});
	}
}
