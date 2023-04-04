// Copyright 2022 Tracer Interactive, LLC. All Rights Reserved.
#include "WebUIModule.h"
#include "Modules/ModuleManager.h"

#if !UE_SERVER
#include "WebInterfaceAssetManager.h"
#include "Materials/Material.h"
#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX
#include "IWebInterfaceBrowserSingleton.h"
#include "WebInterfaceBrowserModule.h"
#else
#include "IWebBrowserSingleton.h"
#include "WebBrowserModule.h"
#endif
#endif

class FWebUIModule : public IWebUIModule
{
public:
	virtual void StartupModule() override
	{
#if !UE_SERVER
		if (WebInterfaceAssetMgr == nullptr)
		{
#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX
		  //IWebInterfaceBrowserModule::Get(); // force the module to load
		  //if (IWebInterfaceBrowserModule::IsAvailable() && IWebInterfaceBrowserModule::Get().IsWebModuleAvailable())
		  //{
		  //	IWebInterfaceBrowserSingleton* WebBrowserSingleton = IWebInterfaceBrowserModule::Get().GetSingleton();
		  //	if (WebBrowserSingleton)
		  //	{
		  //		WebBrowserSingleton->SetDefaultMaterial(WebInterfaceAssetMgr->GetDefaultMaterial());
		  //		WebBrowserSingleton->SetDefaultTranslucentMaterial(WebInterfaceAssetMgr->GetDefaultTranslucentMaterial());
		  //	}
		  //}
#else
			WebInterfaceAssetMgr = NewObject<UWebInterfaceAssetManager>((UObject*)GetTransientPackage(), NAME_None, RF_Transient | RF_Public);
			WebInterfaceAssetMgr->LoadDefaultMaterials();

			IWebBrowserModule::Get(); // force the module to load
			if (IWebBrowserModule::IsAvailable() && IWebBrowserModule::Get().IsWebModuleAvailable())
			{
				IWebBrowserSingleton* WebBrowserSingleton = IWebBrowserModule::Get().GetSingleton();
				if (WebBrowserSingleton)
				{
					WebBrowserSingleton->SetDefaultMaterial(WebInterfaceAssetMgr->GetDefaultMaterial());
					WebBrowserSingleton->SetDefaultTranslucentMaterial(WebInterfaceAssetMgr->GetDefaultTranslucentMaterial());
				}
			}
#endif
		}
#endif
	}

	virtual void ShutdownModule() override
	{
	}

#if !UE_SERVER
private:
	UWebInterfaceAssetManager* WebInterfaceAssetMgr;
#endif
};

IMPLEMENT_MODULE(FWebUIModule, WebUI);
