// Copyright 2022 Tracer Interactive, LLC. All Rights Reserved.
#pragma once
#include "IHttpLibraryRequest.h"
#include "CoreMinimal.h"
#include "Http.h"
#include "HttpLibraryEnums.h"
#include "JsonLibrary.h"
#include "HttpLibraryJsonRequest.generated.h"

struct HTTPLIBRARY_API FHttpLibraryJsonRequest : IHttpLibraryRequest
{
	FHttpLibraryJsonResponse OnResponse;
	FHttpLibraryProgress     OnProgress;

protected:

	virtual bool Process() override;
};

DECLARE_DYNAMIC_DELEGATE_TwoParams( FHttpLibraryRequestOnJsonResponse, int32, StatusCode, const FJsonLibraryValue&, Content );
DECLARE_DYNAMIC_DELEGATE_TwoParams( FHttpLibraryRequestOnJsonProgress, int32, BytesSent, int32, BytesReceived );

UCLASS(BlueprintType, meta = (DisplayName = "HTTP Request (JSON)"))
class HTTPLIBRARY_API UHttpLibraryJsonRequest : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	
	FHttpLibraryRequestOnJsonResponse OnResponse;
	FHttpLibraryRequestOnJsonProgress OnProgress;
	
protected:

	FHttpLibraryJsonRequest Http;

	void TriggerResponse( int32 StatusCode, const FJsonLibraryValue& Content );
	void TriggerProgress( int32 Sent, int32 Received );

public:

	// Check if a HTTP request is in progress.
	UFUNCTION(BlueprintPure, meta = (DisplayName = "In Progress"), Category = "HTTP Library|Request")
	bool IsRunning() const;
	// Check if a HTTP request is complete.
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Is Complete"), Category = "HTTP Library|Request")
	bool IsComplete() const;
	
	// Send a HTTP request.
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Send Request", AdvancedDisplay = "QueryString,Headers,Method", AutoCreateRefTerm = "QueryString,Headers"), Category = "HTTP Library|Request")
	bool Send( const FString& URL, const TMap<FString, FString>& QueryString, const TMap<FString, FString>& Headers, EHttpLibraryRequestMethod Method = EHttpLibraryRequestMethod::GET );
	// Send a HTTP request with content.
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Send Content", AdvancedDisplay = "QueryString,Headers,Method", AutoCreateRefTerm = "QueryString,Headers"), Category = "HTTP Library|Request")
	bool SendString( const FString& URL, const TMap<FString, FString>& QueryString, const TMap<FString, FString>& Headers, const FString& Content, EHttpLibraryContentType ContentType = EHttpLibraryContentType::Default, EHttpLibraryRequestMethod Method = EHttpLibraryRequestMethod::POST );
	// Send a HTTP request with JSON content.
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Send Content (JSON)", AdvancedDisplay = "QueryString,Headers,Method", AutoCreateRefTerm = "QueryString,Headers,Content"), Category = "HTTP Library|Request")
	bool SendJSON( const FString& URL, const TMap<FString, FString>& QueryString, const TMap<FString, FString>& Headers, const FJsonLibraryValue& Content, EHttpLibraryRequestMethod Method = EHttpLibraryRequestMethod::POST );
	// Send a HTTP request with binary content.
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Send Content (Binary)", AdvancedDisplay = "QueryString,Headers,Method", AutoCreateRefTerm = "QueryString,Headers,Content"), Category = "HTTP Library|Request")
	bool SendBinary( const FString& URL, const TMap<FString, FString>& QueryString, const TMap<FString, FString>& Headers, const TArray<uint8>& Content, EHttpLibraryContentType ContentType = EHttpLibraryContentType::Default, EHttpLibraryRequestMethod Method = EHttpLibraryRequestMethod::POST );
	
	// Cancel a HTTP request if currently in progress.
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Cancel"), Category = "HTTP Library|Request")
	bool Cancel();
};
