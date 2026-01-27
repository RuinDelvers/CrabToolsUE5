#pragma once

#include "Components/RPGSystem/RPGProperty.h"
#include "Components/RPGSystem/Properties/IntAttribute.h"
#include "IntResource.generated.h"


UCLASS(Blueprintable, BlueprintType)
class CRABTOOLSUE5_API UIntResource : public URPGResource
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, Category="Resources")
	int Value = 0;

public:

	UIntResource();

	UFUNCTION(BlueprintCallable, Category="RPGProperty",
		meta = (CompactNodeTitle = "->", BlueprintAutocast))
	int GetValue() const { return this->Value; }

	UFUNCTION(BlueprintCallable, Category = "RPGProperty")
	void SetValue(int UValue);

	UFUNCTION(BlueprintCallable, Category = "RPGProperty")
	int GetMin() const { return IIntegerRPGProperty::Execute_GetIntegerValue(this->Minimum); }

	UFUNCTION(BlueprintCallable, Category = "RPGProperty")
	int GetMax() const { return IIntegerRPGProperty::Execute_GetIntegerValue(this->Maximum); }

	virtual float GetPercent_Implementation() const override;

	UFUNCTION(BlueprintCallable, Category = "RPGProperty")
	void Refresh();

	UFUNCTION(BlueprintCallable, Category = "Operations")
	void Increment();

	UFUNCTION(BlueprintCallable, Category = "Operations")
	void Decrement();

	virtual void UseResourceInt_Implementation(int Amount) override;
	virtual void UseResourceFloat_Implementation(float Amount) override;

	virtual bool HasResourceInt_Implementation(int Compare) const override;
	virtual bool HasResourceFloat_Implementation(float Compare) const override;

	virtual void SetMinProp(URPGProperty* Prop) override;
	virtual void SetMaxProp(URPGProperty* Prop) override;

protected:

	virtual FText GetDisplayText_Implementation() const override;
	virtual void Initialize_Inner_Implementation() override;

	virtual TSubclassOf<URPGSetter> GetSetter_Implementation() const override { return UIntegerPropertySetter::StaticClass(); }
	virtual TSubclassOf<URPGCompare> GetCompare_Implementation() const override { return UIntegerPropertyCompare::StaticClass(); }
	virtual TSubclassOf<URPGProperty> GetBoundsType_Implementation() const override { return UIntegerRPGProperty::StaticClass(); }

private:

	UFUNCTION()
	void OnAttributeChanged(URPGProperty* Attr);
};
