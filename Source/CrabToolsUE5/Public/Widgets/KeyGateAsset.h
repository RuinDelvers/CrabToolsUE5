#pragma once

#include "InputCoreTypes.h"
#include "Engine/DataAsset.h"

#include "KeyGateAsset.generated.h"

/* This asset type is used to more cleaning map key inputs in widgets to their functions. */
UCLASS()
class UKeyGateAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	TSet<FKey> ValidKeys;

public:

	UFUNCTION(BlueprintCallable, Category="Input", meta=(ExpandBoolAsExecs="ReturnValue"))
	static bool Route(const UKeyGateAsset* Gate, const FKey& Input);
};