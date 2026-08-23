#pragma once

#include "Properties/GenericPropertyBinding.h"
#include "GenericPropertyBlueprintLibrary.generated.h"

UCLASS()
class UGenericPropertyBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Property", BlueprintPure,
		meta = (DefaultToSelf = "Source", HidePin = "Source", BlueprintAutocast, CompactNodeTitle = "->"))
	static int GetInt(const UObject* Source, const FGenericIntPropertyBinding& Binding);

	UFUNCTION(BlueprintCallable, Category = "Property", BlueprintPure,
		meta = (DefaultToSelf = "Source", HidePin = "Source", BlueprintAutocast, CompactNodeTitle = "->"))
	static float GetFloat(const UObject* Source, const FGenericFloatPropertyBinding& Binding);

	UFUNCTION(BlueprintCallable, Category = "Property", BlueprintPure,
		meta = (DefaultToSelf = "Source", HidePin = "Source", BlueprintAutocast, CompactNodeTitle = "->"))
	static double GetDouble(const UObject* Source, const FGenericDoublePropertyBinding& Binding);

	UFUNCTION(BlueprintCallable, Category="Property", BlueprintPure,
		meta = (DefaultToSelf = "Source", HidePin = "Source", BlueprintAutocast, CompactNodeTitle = "->"))
	static bool GetBool(const UObject* Source, const FGenericBoolPropertyBinding& Binding);

	UFUNCTION(BlueprintCallable, Category = "Property",
		meta = (DefaultToSelf = "Source", HidePin = "Source", ExpandBoolAsExecs="ReturnValue"))
	static bool BranchBool(const UObject* Source, const FGenericBoolPropertyBinding& Binding);
};