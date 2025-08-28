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

	UFUNCTION(BlueprintCallable, Category = "RPGProperty")
	void Operate(UFloatOperator* Op);

	UFUNCTION(BlueprintCallable, Category = "RPGProperty")
	void UnOperate(UFloatOperator* Op);

	UFUNCTION(BlueprintCallable, Category = "RPGProperty")
	void Refresh();

protected:

	virtual void Initialize_Inner_Implementation() override;
	virtual FText GetDisplayText_Implementation() const override;

	virtual void ListenToProperty_Implementation(const FRPGPropertyChanged& Callback) override
	{
		OnAttributeChanged.Add(Callback);
	}

	virtual void StopListeningToProperty_Implementation(UObject* Obj) override
	{
		OnAttributeChanged.RemoveAll(Obj);
	}
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

UCLASS(Blueprintable, DefaultToInstanced, CollapseCategories, EditInlineNew)
class CRABTOOLSUE5_API UFloatOperator : public UObject
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	int Priority;

	URPGComponent* Owner;

public:

	UFUNCTION(BlueprintNativeEvent, Category = "RPG|Operators")
	float Operate(float Value);
	virtual float Operate_Implementation(float Value) { return Value; }

	UFUNCTION(BlueprintNativeEvent, Category = "RPG|Operators")
	void Initialize();
	virtual void Initialize_Implementation() {}


	int GetPriority() const { return this->Priority; }

	void SetOwner(URPGComponent* UOwner) { this->Owner = UOwner; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RPG|Operators")
	URPGComponent* GetOwner() const { return this->Owner; }
};