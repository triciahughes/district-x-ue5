// Copyright 2022 Tracer Interactive, LLC. All Rights Reserved.
#include "WebInterfaceSchemeHandler.h"
#include "WebInterfaceHelpers.h"
#if !UE_SERVER && WITH_CEF3
#include "Async/Async.h"
#include "GenericPlatform/GenericPlatformHttp.h"
#include "HAL/FileManager.h"
#include "Http.h"

#define BIN "application/octet-stream"
FWebInterfaceSchemeHandler::FWebInterfaceSchemeHandler()
	: MimeType( BIN )
	, ContentLength( 0 )
	, TotalBytesRead( 0 )
	, Reader( nullptr )
{
}

FWebInterfaceSchemeHandler::~FWebInterfaceSchemeHandler()
{
	CloseReader();
}

bool FWebInterfaceSchemeHandler::ProcessRequest( const FString& Verb, const FString& Url, const FSimpleDelegate& OnHeadersReady )
{
	if ( Verb.ToUpper() != "GET" )
		return false;

	FString FilePath = Url;
	if ( FilePath.Contains( "://" ) )
	{
		FString Scheme;
		if ( FParse::SchemeNameFromURI( *FilePath, Scheme ) )
			FilePath = FilePath.RightChop( Scheme.Len() + 3 );
	}

	int32 DesiredWidth  = 0;
	int32 DesiredHeight = 0;

	int32 IndexHash  = FilePath.Find( "#" );
	if ( IndexHash >= 0 )
		FilePath = FilePath.Left( IndexHash );

	int32 IndexQuery = FilePath.Find( "?" );
	if ( IndexQuery >= 0 )
	{
		TArray<FString> QueryParts;
		FilePath.RightChop( IndexQuery + 1 ).ParseIntoArray( QueryParts, TEXT( "&" ), true );

		TMap<FString, FString> QueryString;
		for ( int32 i = 0; i < QueryParts.Num(); i++ )
		{
			FString LHS, RHS;
			QueryParts[i].Split( "=", &LHS, &RHS );

			LHS = FPlatformHttp::UrlDecode( LHS );
			RHS = FPlatformHttp::UrlDecode( RHS );

			QueryString.Add( LHS, RHS );
		}

		if ( QueryString.Contains( "w" ) )
			DesiredWidth = FCString::Strtoi( *QueryString[ "w" ], nullptr, 10 );
		if ( QueryString.Contains( "h" ) )
			DesiredHeight = FCString::Strtoi( *QueryString[ "h" ], nullptr, 10 );

		FilePath = FilePath.Left( IndexQuery );
	}

	const FString ContentDirectory = FPaths::ProjectContentDir();
	FilePath = ContentDirectory + FilePath;
	FilePath = FilePath.Replace( TEXT( "\\" ), TEXT( "/" ) );
	FilePath = FilePath.Replace( TEXT( "//" ), TEXT( "/" ) );

	int64 FileSize = IFileManager::Get().FileSize( *FilePath );
	if ( FileSize != INDEX_NONE )
	{
		ContentLength = (int32)FileSize;
		if ( FileSize > INT32_MAX )
			return false;
	
		MimeType = FGenericPlatformHttp::GetMimeType( FilePath );
		if ( MimeType.Len() == 0 || MimeType == "application/unknown" )
			MimeType = BIN;

		CreateReader( *FilePath );
		OnHeadersReady.Execute();
	}
	else
	{
		FString FileName      = FPaths::GetBaseFilename( FilePath );
		FString FileExtension = FPaths::GetExtension( FilePath ).ToLower();
		if ( FileExtension == "png" )
			ImageFormat = EImageFormat::PNG;
		else if ( FileExtension == "jpg"
			   || FileExtension == "jpeg" )
			ImageFormat = EImageFormat::JPEG;
		else
			ImageFormat = EImageFormat::Invalid;

		MimeType = FGenericPlatformHttp::GetMimeType( FilePath );
		if ( MimeType.Len() == 0 || MimeType == "application/unknown" )
			MimeType = BIN;
		
		if ( ImageFormat != EImageFormat::Invalid )
		{
			FilePath  = "/Game" / FilePath.RightChop( ContentDirectory.Len() );
			FilePath  = FilePath.LeftChop( FileExtension.Len() + 1 );
			FilePath += "." + FileName;

			AsyncTask( ENamedThreads::GameThread, [this, FilePath, DesiredWidth, DesiredHeight, OnHeadersReady]()
			{
				if ( !CreateImageReader( FilePath, DesiredWidth, DesiredHeight ) )
					ImageFormat = EImageFormat::Invalid;

				OnHeadersReady.Execute();
			} );
		}
		else
			OnHeadersReady.Execute();
	}

	return true;
}

void FWebInterfaceSchemeHandler::GetResponseHeaders( IHeaders& OutHeaders )
{
	if ( ImageSize.X > 0 && ImageSize.Y > 0 && ImagePixels.Num() > 0 && ImageFormat != EImageFormat::Invalid )
	{
		ContentLength = (int32)GetCompressedImageData();
		if ( ContentLength <= 0 )
			MimeType = BIN;
	}

	ImagePixels.Empty();
	if ( Reader || ImageData.Num() > 0 )
	{
		OutHeaders.SetStatusCode( 200 );
		OutHeaders.SetMimeType( *MimeType );
		OutHeaders.SetContentLength( ContentLength );
	}
	else
		OutHeaders.SetStatusCode( 404 );
}

bool FWebInterfaceSchemeHandler::ReadResponse( uint8* OutBytes, int32 BytesToRead, int32& BytesRead, const FSimpleDelegate& OnMoreDataReady )
{
	BytesRead = 0;
	if ( !Reader && ImageData.Num() <= 0 )
		return false;

	BytesRead = ContentLength - TotalBytesRead;
	if ( BytesRead <= 0 )
		return false;

	if ( BytesRead > BytesToRead )
		BytesRead = BytesToRead;

	if ( ImageData.Num() > 0 )
		FMemory::Memcpy( OutBytes, ImageData.GetData() + TotalBytesRead, BytesRead);
	else
		Reader->Serialize( OutBytes, BytesRead );

	TotalBytesRead += BytesRead;
	if ( TotalBytesRead < ContentLength )
		OnMoreDataReady.Execute();
	else
		CloseReader();
	
	return true;
}

void FWebInterfaceSchemeHandler::Cancel()
{
	CloseReader();
}

bool FWebInterfaceSchemeHandler::CreateReader( const FString& FilePath )
{
	if ( Reader )
		CloseReader();

	Reader = IFileManager::Get().CreateFileReader( *FilePath );
	if ( !Reader )
		return false;

	return true;
}

void FWebInterfaceSchemeHandler::CloseReader()
{
	MimeType = BIN;

	ContentLength  = 0;
	TotalBytesRead = 0;

	ImageFormat = EImageFormat::Invalid;
	ImageSize   = FIntPoint::ZeroValue;

	ImagePixels.Empty();
	ImageData.Empty();

	if ( !Reader )
		return;

	Reader->Close();

	delete Reader;
	Reader = nullptr;
}

bool FWebInterfaceSchemeHandler::CreateImageReader( const FString& FilePath, int32 DesiredWidth, int32 DesiredHeight )
{
	TSoftObjectPtr AssetPtr;
	AssetPtr = FilePath;

	check( IsInGameThread() );

	UObject* Asset = AssetPtr.LoadSynchronous();
	if ( Asset )
	{
		UTexture2D* Texture = Cast<UTexture2D>( Asset );
		if ( Texture )
		{
			ImageSize = UWebInterfaceHelpers::GenerateImageFromTexture( ImagePixels, Texture );
			ResizeImageReader( DesiredWidth, DesiredHeight );

			return true;
		}

		UMaterialInterface* Material = Cast<UMaterialInterface>( Asset );
		if ( Material )
		{
			ImageSize = UWebInterfaceHelpers::GenerateImageFromMaterial( ImagePixels, Material, DesiredWidth, DesiredHeight );
			return true;
		}

		UTextureRenderTarget2D* RenderTarget = Cast<UTextureRenderTarget2D>( Asset );
		if ( RenderTarget )
		{
			ImageSize = UWebInterfaceHelpers::GenerateImageFromRenderTarget( ImagePixels, RenderTarget );
			ResizeImageReader( DesiredWidth, DesiredHeight );

			return true;
		}
	}

	return false;
}

bool FWebInterfaceSchemeHandler::ResizeImageReader( int32 DesiredWidth, int32 DesiredHeight )
{
	int32 Width  = DesiredWidth;
	int32 Height = DesiredHeight;

	if ( Width > 0 && Height > 0 )
	{
		if ( Width == ImageSize.X && Height == ImageSize.Y )
			return false;
	}
	else if ( Width > 0 )
	{
		if ( Width == ImageSize.X )
			return false;

		Height = FMath::RoundToInt32( Width * ( (float)ImageSize.Y / (float)ImageSize.X ) );
		if ( Height <= 0 )
			return false;
	}
	else if ( Height > 0 )
	{
		if ( Height == ImageSize.Y )
			return false;

		Width = FMath::RoundToInt32( Height * ( (float)ImageSize.X / (float)ImageSize.Y ) );
		if ( Width <= 0 )
			return false;
	}
	else
		return false;
	
	ImagePixels = UWebInterfaceHelpers::ResizeImage( ImageSize.X, ImageSize.Y, ImagePixels, Width, Height );
	ImageSize   = FIntPoint( Width, Height );

	return true;
}

int64 FWebInterfaceSchemeHandler::GetCompressedImageData()
{
	if ( ImageFormat == EImageFormat::Invalid )
		return 0;
	if ( ImageSize.X <= 0 || ImageSize.Y <= 0 )
		return 0;
	if ( ImagePixels.Num() <= 0 )
		return 0;

	IImageWrapper* ImageWrapper = UWebInterfaceHelpers::FindOrCreateImageWrapper( ImageFormat );
	if ( !ImageWrapper )
		return 0;

	if ( UWebInterfaceHelpers::SetImageWrapper( ImageWrapper, ImagePixels, ImageSize ) )
		ImageData = ImageWrapper->GetCompressed();

	UWebInterfaceHelpers::ReturnImageWrapper( ImageWrapper );
	return ImageData.Num();
}


TUniquePtr<IWebInterfaceBrowserSchemeHandler> FWebInterfaceSchemeHandlerFactory::Create( FString Verb, FString Url )
{
	return MakeUnique<FWebInterfaceSchemeHandler>();
}
#endif
