// Copyright 2022 Tracer Interactive, LLC. All Rights Reserved.
#pragma once
#if !UE_SERVER && WITH_CEF3
#include "IWebInterfaceBrowserSchemeHandler.h"
#include "IImageWrapper.h"

class WEBUI_API FWebInterfaceSchemeHandler : public IWebInterfaceBrowserSchemeHandler
{
public:
	FWebInterfaceSchemeHandler();
	virtual ~FWebInterfaceSchemeHandler();
	
	virtual bool ProcessRequest( const FString& Verb, const FString& Url, const FSimpleDelegate& OnHeadersReady ) override;
	virtual void GetResponseHeaders( IHeaders& OutHeaders ) override;
	virtual bool ReadResponse( uint8* OutBytes, int32 BytesToRead, int32& BytesRead, const FSimpleDelegate& OnMoreDataReady ) override;
	virtual void Cancel() override;

protected:
	FString MimeType;

	int32 ContentLength;
	int32 TotalBytesRead;

	FArchive* Reader;

	bool CreateReader( const FString& FilePath );
	void CloseReader();

	EImageFormat     ImageFormat;
	FIntPoint        ImageSize;
	TArray64<FColor> ImagePixels;
	TArray64<uint8>  ImageData;

	bool  CreateImageReader( const FString& FilePath, int32 DesiredWidth, int32 DesiredHeight );
	bool  ResizeImageReader( int32 DesiredWidth, int32 DesiredHeight );
	int64 GetCompressedImageData();
};

class WEBUI_API FWebInterfaceSchemeHandlerFactory : public IWebInterfaceBrowserSchemeHandlerFactory
{
	virtual TUniquePtr<IWebInterfaceBrowserSchemeHandler> Create( FString Verb, FString Url ) override;
};
#endif
