#pragma once

#include <limits>
#include "Components/RPGSystem/RPGProperty.h"
#include "FloatAttribute.generated.h"

class UIntOperator;

UCLASS(Abstract, NotBlueprintType)
class CRABTOOLSUE5_API UBaseFloatAttribute : public URPGProperty
{
	GENERATED_BODY()

private:

protected:

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "RPG|Attributes", meta = (AllowPrivateAccess = true))
	TArray<TObjectPtr<UFloatOperator>> Operators;

	UPROPERTY()
	float CompValue;

public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeChanged, UBaseFloatAttribute*, Attribute);

	UPROPERTY(BlueprintAssignable, Category = "RPGProperty")
	FAttributeChanged OnAttributeChanged;

public:

	UFUNCTION(BlueprintCallable, Category = "RPGProperty", meta = (CompactNodeTitle = "->"))
	float GetValue() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "RPGProperty")
	float GetBaseValue() const;
	virtual float GetBaseValue_Implementation() const { return 0; }

	virtual void Initialize_Inner_Implementation() override;

	UFUNCTION(BlueprintCallable, Category = "RPGProperty")
	void Operate(UFloatOperator* Op);

	UFUNCTION(BlueprintCallable, Category = "RPGProperty")
	void UnOperate(UFloatOperator* Op);

	UFUNCTION(BlueprintCallable, Category = "RPGProperty")
	void Refresh();
};

UCLASS(NotBlueprintable, DisplayName = "Zero")
class CRABTOOLSUE5_API UZeroFloatAttribute : public UBaseFloatAttribute
{
	GENERATED_BODY()


public:

	virtual float GetBaseValue_Implementation() const override { return 0; }
};

UCLASS(NotBlueprintable, DisplayName = "NegativeInfinity")
class CRABTOOLSUE5_API UMinFloatAttribute : public UBaseFloatAttribute
{
	GENERATED_BODY()


public:

	virtual float GetBaseValue_Implementation() const override { return -std::numeric_limits<float>::infinity(); }
};

UCLASS(NotBlueprintable, DisplayName = "Infinity")
class CRABTOOLSUE5_API UMaxFloatAttribute : public UBaseFloatAttribute
{
	GENERATED_BODY()


public:

	virtual float GetBaseValue_Implementation() const override { return std::numeric_limits<float>::infinity(); }
};

UCLASS(Blueprintable, BlueprintType)
class CRABTOOLSUE5_API UFloatAttribute : public UBaseFloatAttribute
{
	GENERATED_BODY()

private:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RPG|Attributes", meta = (AllowPrivateAccess = true))
	float BaseValue = 0;

public:

	UFUNCTION(BlueprintCallable, Category = "RPGProperty")
	void SetBaseValue(float UValue);

	virtual float GetBaseValue_Implementation() const override { return this->BaseValue; }
};