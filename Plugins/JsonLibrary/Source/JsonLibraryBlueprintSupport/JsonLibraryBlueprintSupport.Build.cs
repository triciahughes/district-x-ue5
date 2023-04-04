// Copyright 2022 Tracer Interactive, LLC. All Rights Reserved.
namespace UnrealBuildTool.Rules
{
	public class JsonLibraryBlueprintSupport : ModuleRules
	{
		public JsonLibraryBlueprintSupport(ReadOnlyTargetRules Target) : base(Target)
		{
			PrivateDependencyModuleNames.AddRange(
				new string[]
				{ 
					"Core", 
					"CoreUObject", 
					"Engine",
					"UnrealEd",
					"Slate",
					"SlateCore",
					"KismetWidgets",
					"KismetCompiler",
					"BlueprintGraph",
					"JsonLibrary"
				}
			);
		}
	}
}
