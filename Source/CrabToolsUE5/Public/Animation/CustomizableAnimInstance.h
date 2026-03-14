#pragma once

#include "Animation/AnimInstanceProxy.h"
#include "CustomizableAnimInstance.generated.h"

/* A delegate used for callbacks that care about float parameter customization. For example, sliders. */
DECLARE_DELEGATE_OneParam(FCustomizableFloatListener, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FCustomizableFloatListener_Multi, float);

UINTERFACE(MinimalAPI, Blueprintable, BlueprintType)
class UCustomizableAnimInstanceInterface : public UInterface
{
	GENERATED_BODY()
};

class ICustomizableAnimInstanceInterface
{
	GENERATED_BODY()

public:

	virtual void RegisterFloatValue(FName ValueKey, const FCustomizableFloatListener& Callback) {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="AlphaCurves")
	TSet<FName> GetAlphaCurveKeys() const;
	virtual TSet<FName> GetAlphaCurveKeys_Implementation() const { return {}; }	

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AlphaCurves")
	void UpdateFloatValue(FName ValueKey, float NewValue);
	virtual void UpdateFloatValue_Implementation(FName ValueKey, float NewValue) {}
};

USTRUCT()
struct FCustomizableValueContainer
{
	GENERATED_BODY()

	TMap<FName, FCustomizableFloatListener_Multi> FloatListeners;
	TMap<FName, float> UpdatedFloatListeners;

public:

	void PushUpdates();
	void ClearUpdates();

	void RegisterFloatValue(FName ValueKey, const FCustomizableFloatListener& Callback);
	void UpdateFloatValue(FName ValueKey, float NewValue);
};


UCLASS(Blueprintable, BlueprintType)
class UCustomizableAnimInstance : public UAnimInstance, public ICustomizableAnimInstanceInterface
{
	GENERATED_BODY()

private:

	UPROPERTY(Transient)
	FCustomizableValueContainer Customization;

public:

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	virtual TSet<FName> GetAlphaCurveKeys_Implementation() const override;
	virtual void RegisterFloatValue(FName ValueKey, const FCustomizableFloatListener& Callback) override;
	virtual void UpdateFloatValue_Implementation(FName ValueKey, float NewValue) override;
};