#pragma once

#include "CoreMinimal.h"
#include "Components/RPGSystem/RPGComponent.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RPGHelpers.generated.h"

/**
 * 
 */
UCLASS()
class URPGHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	#pragma region Integer Attributes & Resources
	UFUNCTION(BlueprintCallable, Category="UtilityFunctions")
	static void ObserveIntAttribute(UPARAM(ref) FIntAttribute& input, const FIntAttributeCallback& Callback, bool bRemove=false);

	UFUNCTION(BlueprintCallable, Category = "UtilityFunctions")
	static void ObserveIntResource(UPARAM(ref) FIntResource& input, const FIntAttributeCallback& Callback, bool bRemove = false);

	UFUNCTION(BlueprintCallable, Category = "UtilityFunctions")
	static void SetIntResource(UPARAM(ref) FIntResource& input, int Value);

	UFUNCTION(Blueprintpure, Category="UtilityFunctions",
		meta=(DisplayName="UnwrapInt", CompactNodeTitle="->", BlueprintAutocast))
	static int UnwrapIntAttribute(const FIntAttribute& input);

	UFUNCTION(Blueprintpure, Category = "UtilityFunctions",
		meta = (DisplayName = "UnwrapInt", CompactNodeTitle = "->", BlueprintAutocast))
	static int UnwrapIntResource(const FIntResource& input);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UtilityFunctions")
	static int MaxIntRescValue(const FIntResource& Input);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UtilityFunctions")
	static int MinIntRescValue(const FIntResource& Input);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UtilityFunctions")
	static float IntRescPercent(const FIntResource& Input);

	#pragma endregion

	#pragma region Float Attributes & Resources
	UFUNCTION(BlueprintCallable, Category = "UtilityFunctions")
	static void ObserveFloatAttribute(UPARAM(ref) FFloatAttribute& input, const FFloatAttributeCallback& Callback, bool bRemove = false);

	UFUNCTION(BlueprintCallable, Category = "UtilityFunctions")
	static void ObserveFloatResource(UPARAM(ref) FFloatResource& input, const FFloatAttributeCallback& Callback, bool bRemove = false);

	UFUNCTION(BlueprintCallable, Category = "UtilityFunctions")
	static void SetFloatResource(UPARAM(ref) FFloatResource& input, float Value);

	UFUNCTION(Blueprintpure, Category = "UtilityFunctions",
		meta = (DisplayName = "UnwrapFloat", CompactNodeTitle = "->", BlueprintAutocast))
	static float UnwrapFloatAttribute(const FFloatAttribute& input);

	UFUNCTION(Blueprintpure, Category = "UtilityFunctions",
		meta = (DisplayName = "UnwrapFloat", CompactNodeTitle = "->", BlueprintAutocast))
	static float UnwrapFloatResource(const FFloatResource& input);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UtilityFunctions")
	static float MaxFloatRescValue(const FFloatResource& Input);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UtilityFunctions")
	static float MinFloatRescValue(const FFloatResource& Input);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UtilityFunctions")
	static float FloatRescPercent(const FFloatResource& Input);

	#pragma endregion
};