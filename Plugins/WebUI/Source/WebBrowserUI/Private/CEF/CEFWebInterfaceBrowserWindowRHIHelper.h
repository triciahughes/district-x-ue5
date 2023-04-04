// Engine/Source/Runtime/WebBrowser/Private/CEF/CEFWebInterfaceBrowserWindowRHIHelper.h

#pragma once

#include "CoreMinimal.h"

#if WITH_CEF3

#include "Layout/Geometry.h"

class FSlateTexture2DRHIRef;
class FSlateUpdatableTexture;

/**
 * Implementation of RHI renderer details for the CEF accelerated rendering path
 */
class FCEFWebInterfaceBrowserWindowRHIHelper
{

public:
	/** Virtual Destructor. */
	virtual ~FCEFWebInterfaceBrowserWindowRHIHelper();


public:
	static bool BUseRHIRenderer();
	FSlateUpdatableTexture* CreateTexture(void *ShareHandle);
	void UpdateSharedHandleTexture(void* SharedHandle, FSlateUpdatableTexture* SlateTexture, const FIntRect& DirtyIn);
	void UpdateCachedGeometry(const FGeometry& AllottedGeometry);
	TOptional<FSlateRenderTransform> GetWebBrowserRenderTransform() const;

private:
	FGeometry AllottedGeometry;
};

#endif
