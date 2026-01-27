#pragma once

#include "Components/RPGSystem/RPGProperty.h"
#include "Components/RPGSystem/Properties/FloatAttribute.h"
#include "FloatResource.generated.h"

UCLASS(Blueprintable, BlueprintType)
class CRABTOOLSUE5_API UFloatResource : public URPGResource
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, Category = "Resources")
	float Value = 0;

public:

	UFloatResource();

	UFUNCTION(BlueprintCallable, Category = "RPGProperty",
		meta = (CompactNodeTitle = "->", BlueprintAutocast))
	float GetValue() const { return this->Value; }

	UFUNCTION(BlueprintCallable, Category = "RPGProperty")
	void SetValue(float UValue);

	UFUNCTION(BlueprintCallable, Category = "RPGProperty")
	float GetMin() const { return IFloatRPGProperty::Execute_GetFloatValue(this->Minimum); }

	UFUNCTION(BlueprintCallable, Category = "RPGProperty")
	float GetMax() const { return IFloatRPGProperty::Execute_GetFloatValue(this->Maximum); }

	virtual float GetPercent_Implementation() const override;

	UFUNCTION(BlueprintCallable, Category = "RPGProperty")
	void Refresh();


	virtual void UseResourceInt_Implementation(int Amount) override;
	virtual void UseResourceFloat_Implementation(float Amount) override;
	virtual bool HasResourceInt_Implementation(int Compare) const override;
	virtual bool HasResourceFloat_Implementation(float Compare) const override;

	virtual void SetMinProp(URPGProperty* Prop) override;
	virtual void SetMaxProp(URPGProperty* Prop) override;

protected:

	virtual TSubclassOf<URPGSetter> GetSetter_Implementation() const override { return UFloatPropertySetter::StaticClass(); }
	virtual TSubclassOf<URPGCompare> GetCompare_Implementation() const override { return UFloatPropertyCompare::StaticClass(); }
	virtual TSubclassOf<URPGProperty> GetBoundsType_Implementation() const override { return UFloatRPGProperty::StaticClass(); }

	virtual FText GetDisplayText_Implementation() const override;
	virtual void Initialize_Inner_Implementation() override;

private:

	UFUNCTION()
	void OnAttributeChanged(URPGProperty* Attr);
};
