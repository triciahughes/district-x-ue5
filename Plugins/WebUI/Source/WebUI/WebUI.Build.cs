// Copyright 2022 Tracer Interactive, LLC. All Rights Reserved.
namespace UnrealBuildTool.Rules
{
	public class WebUI : ModuleRules
	{
		public WebUI(ReadOnlyTargetRules Target) : base(Target)
		{
			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
					"CoreUObject",
					"Engine",
                    "InputCore",
                    "RenderCore",
                    "RHI",
                    "Slate",
					"SlateCore",
					"UMG",
					"ImageWriteQueue",
					"JsonLibrary"
				}
			);

			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"HTTP"
				}
			);

            if (Target.bBuildEditor == true)
            {
            	PrivateIncludePathModuleNames.AddRange(
                    new string[]
                    {
                        "UnrealEd",
                    }
                );
                
                PrivateDependencyModuleNames.AddRange(
                    new string[]
                    {
						"EditorFramework",
                        "UnrealEd",
                    }
                );
            }

			if (Target.Type != TargetType.Server)
			{
				if (Target.Platform == UnrealTargetPlatform.Win64
				||  Target.Platform == UnrealTargetPlatform.Mac
				||  Target.Platform == UnrealTargetPlatform.Linux)
				{
					PublicDependencyModuleNames.AddRange(
						new string[]
						{
							"WebBrowserUI"
						}
					);
				}
				else
				{
					PublicDependencyModuleNames.AddRange(
						new string[]
						{
							"WebBrowser"
						}
					);
				}

				if (Target.bBuildEditor || Target.Platform == UnrealTargetPlatform.Android || Target.Platform == UnrealTargetPlatform.IOS)
				{
					PrivateIncludePathModuleNames.AddRange(
						new string[]
						{
							"WebBrowserTexture"
						}
					);

					PrivateDependencyModuleNames.AddRange(
						new string[]
						{
							"WebBrowserTexture"
						}
					);
				}
			}
		}
	}
}
