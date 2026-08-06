#pragma once

#include "Properties/GenericPropertyBinding.h"
#include "GenericPropertyBlueprintLibrary.generated.h"

UCLASS()
class UGenericPropertyBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="Property",
		meta = (DefaultToSelf = "Source", HidePin = "Source", BlueprintAutocast, CompactNodeTitle = "->"))
	static bool GetBool(const UObject* Source, const FGenericBoolPropertyBinding& Binding);

	UFUNCTION(BlueprintCallable, Category = "Property",
		meta = (DefaultToSelf = "Source", HidePin = "Source", ExpandBoolAsExecs="ReturnValue"))
	static bool BranchtBool(const UObject* Source, const FGenericBoolPropertyBinding& Binding);
};