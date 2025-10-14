using UnrealBuildTool;

public class CrabToolsUE5 : ModuleRules
{
	public CrabToolsUE5(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			});
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			});
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"EnhancedInput",
				"UMG",
				"Slate",
				"SlateCore",
				"InputCore",
				"GameplayTags",
				"AIModule",
				"DeveloperSettings",
				"LevelSequence",
				"MovieScene",
			});
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"NavigationSystem",
                "LevelSequence",
            });
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			});
	}
}
