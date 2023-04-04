// Engine/Source/Runtime/WebBrowser/Private/Native/NativeJSStructDeserializerBackend.h

#pragma once

#include "CoreMinimal.h"
#include "NativeInterfaceJSScripting.h"
#include "Backends/JsonStructDeserializerBackend.h"
#include "Serialization/MemoryReader.h"

class FNativeInterfaceJSStructDeserializerBackend
	: public FJsonStructDeserializerBackend
{
public:
	FNativeInterfaceJSStructDeserializerBackend(FNativeInterfaceJSScriptingRef InScripting, FMemoryReader& Reader);

	virtual bool ReadProperty( FProperty* Property, FProperty* Outer, void* Data, int32 ArrayIndex ) override;

private:
	FNativeInterfaceJSScriptingRef Scripting;

};
