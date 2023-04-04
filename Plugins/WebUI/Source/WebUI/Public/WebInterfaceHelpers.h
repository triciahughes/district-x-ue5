// Copyright 2022 Tracer Interactive, LLC. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "IImageWrapper.h"
#include "JsonLibrary.h"
#include "WebInterfaceCallback.h"
#include "WebInterfaceHelpers.generated.h"

UCLASS()
class WEBUI_API UWebInterfaceHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

#if UE_EDITOR
protected:
#else
public:
#endif

	// Check if this callback is valid.
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Valid"), Category = "Web UI|Callback")
	static bool WebInterfaceCallback_IsValid( UPARAM(ref) const FWebInterfaceCallback& Target );
	// Call ue.interface.callback(data) in the browser context.
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Call", AdvancedDisplay = "Data", AutoCreateRefTerm = "Data"), Category = "Web UI|Callback")
	static void WebInterfaceCallback_Call( UPARAM(ref) FWebInterfaceCallback& Target, const FJsonLibraryValue& Data );

public:

	static TArray64<FColor> ResizeImage( int32 SrcWidth, int32 SrcHeight, const TArray64<FColor>& SrcData, int32 DstWidth, int32 DstHeight, bool bLinearSpace = false, bool bForceOpaqueOutput = false );

	static IImageWrapper* FindOrCreateImageWrapper( EImageFormat Format );
	static bool           SetImageWrapper( IImageWrapper* Wrapper, const TArray64<FColor>& Pixels, FIntPoint Size );
	static void           ReturnImageWrapper( IImageWrapper* Wrapper );

	static FIntPoint GenerateImageFromRenderTarget( TArray64<FColor>& OutPixels, class UTextureRenderTarget2D* RenderTarget );
	static FIntPoint GenerateImageFromTexture( TArray64<FColor>& OutPixels, class UTexture2D* Texture );
	static FIntPoint GenerateImageFromMaterial( TArray64<FColor>& OutPixels, class UMaterialInterface* Material, int32 Width, int32 Height );

private:

	static bool GenerateImageFromCamera( TArray64<FColor>& OutPixels, UObject* WorldContextObject, const FTransform& CameraTransform, int32 Width, int32 Height, float FOVDegrees = 50.0f, float MinZ = 50.0f, float Gamma = 2.2f );
	static bool GenerateImageFromScene( TArray64<FColor>& OutPixels, FSceneInterface* Scene, const FVector& ViewOrigin, const FMatrix& ViewRotationMatrix, const FMatrix& ProjectionMatrix, FIntPoint TargetSize, float TargetGamma );
};
