#pragma once

#include <limits>
#include "Components/RPGSystem/RPGProperty.h"
#include "FloatAttribute.generated.h"

class UFloatOperator;

UINTERFACE(Blueprintable)
class UFloatRPGProperty : public UInterface
{
	GENERATED_BODY()
};

class IFloatRPGProperty
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, Category = "RPGProperty|Float")
	float GetFloatValue() const;
	virtual float GetFloatValue_Implementation() const { return 0; }

	UFUNCTION(BlueprintNativeEvent, Category = "RPGProperty|Float")
	void SetFloatValue(float NewValue);
	virtual void SetFloatValue_Implementation(float NewValue) {}
};

UCLASS(Abstract, NotBlueprintType)
class CRABTOOLSUE5_API UBaseFloatAttribute : public URPGProperty, public IFloatRPGProperty
{
	GENERATED_BODY()

private:

protected:

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Attributes", meta = (AllowPrivateAccess = true))
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

	virtual TSubclassOf<URPGSetter> GetSetter_Implementation() const override;
	virtual TSubclassOf<URPGCompare> GetCompare_Implementation() const override;

	virtual float GetFloatValue_Implementation() const override { return this->GetValue(); }
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes", meta = (AllowPrivateAccess = true))
	float BaseValue = 0;

public:

	UFUNCTION(BlueprintCallable, Category = "RPGProperty")
	void SetBaseValue(float UValue);

protected:

	virtual void SetFloatValue_Implementation(float NewValue) override { this->SetBaseValue(NewValue); }
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


/*
 *  Operation for IntAttributes, Comparisons happen in the order Property OP Value.
 */
UCLASS(Blueprintable)
class UFloatPropertySetter : public URPGSetter
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPGProperty",
		meta = (AllowPrivateAccess))
	bool bInline = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPGProperty",
		meta = (AllowPrivateAccess, EditCondition = "bInline", EditConditionHides))
	float Value = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPGProperty",
		meta = (AllowPrivateAccess, EditCondition = "bInline", EditConditionHides,
			GetOptions = "GetPropertyNames"))
	FName SourcePropertyName;

	UPROPERTY()
	TObjectPtr<URPGProperty> SourceProperty;

public:

	virtual void ApplyValue_Implementation() override;
	virtual UClass* GetPropertySearchType_Implementation() const override
	{
		return UFloatRPGProperty::StaticClass();
	}

protected:

	virtual void Initialize_Inner_Implementation() override;

};

/*
 *  Operation for IntAttributes, Comparisons happen in the order Property OP Value.
 */
UCLASS(Blueprintable)
class UFloatPropertyCompare : public URPGCompare
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPGProperty",
		meta = (AllowPrivateAccess))
	ENumericComparison Comparison;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPGProperty",
		meta = (AllowPrivateAccess))
	bool bInline = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPGProperty",
		meta = (AllowPrivateAccess, EditCondition = "bInline", EditConditionHides))
	float Value = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPGProperty",
		meta = (AllowPrivateAccess, EditCondition = "!bInline", EditConditionHides,
			GetOptions = "GetPropertyNames"))
	FName ComparePropertyName;

	UPROPERTY()
	TObjectPtr<URPGProperty> CompareProperty;

public:

	virtual bool Compare_Implementation() override;
	virtual UClass* GetPropertySearchType_Implementation() const override
	{
		return UFloatRPGProperty::StaticClass();
	}

protected:

	virtual void Initialize_Inner_Implementation() override;

};