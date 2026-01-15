#pragma once

#include "Utils/Enums.h"
#include "Components/RPGSystem/RPGProperty.h"
#include "IntAttribute.generated.h"

class UIntOperator;

UINTERFACE(Blueprintable)
class UIntegerRPGProperty : public UInterface
{
	GENERATED_BODY()
};

class IIntegerRPGProperty
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, Category="RPGProperty|Integer")
	int GetIntegerValue() const;
	virtual int GetIntegerValue_Implementation() const { return 0; }

	UFUNCTION(BlueprintNativeEvent, Category = "RPGProperty|Integer")
	void SetIntegerValue(int NewValue);
	virtual void SetIntegerValue_Implementation(int NewValue) {}
};

UCLASS(Abstract, NotBlueprintType)
class CRABTOOLSUE5_API UBaseIntAttribute: public URPGProperty, public IIntegerRPGProperty
{
	GENERATED_BODY()

private:

protected:

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "RPG|Attributes", meta = (AllowPrivateAccess = true))
	TArray<TObjectPtr<UIntOperator>> Operators;

	UPROPERTY()
	int CompValue = 0;

public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeChanged, UBaseIntAttribute*, Attribute);

	UPROPERTY(BlueprintAssignable, Category="RPGProperty")
	FAttributeChanged OnAttributeChanged;

public:

	UFUNCTION(BlueprintCallable, Category="RPGProperty", meta = (CompactNodeTitle = "->"))
	int GetValue() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "RPGProperty")
	int GetBaseValue() const;
	virtual int GetBaseValue_Implementation() const { return 0; }
	

	UFUNCTION(BlueprintCallable, Category = "RPGProperty")
	void Operate(UIntOperator* Op);

	UFUNCTION(BlueprintCallable, Category = "RPGProperty")
	void UnOperate(UIntOperator* Op);

	UFUNCTION(BlueprintCallable, Category = "RPGProperty")
	void Refresh();

	virtual TSubclassOf<URPGSetter> GetSetter_Implementation() const override;
	virtual TSubclassOf<URPGCompare> GetCompare_Implementation() const override;
	virtual int GetIntegerValue_Implementation() const override { return this->GetValue(); }

protected:

	virtual void ListenToProperty_Implementation(const FRPGPropertyChanged& Callback) override
	{
		OnAttributeChanged.Add(Callback);
	}

	virtual void StopListeningToProperty_Implementation(UObject* Obj) override
	{
		OnAttributeChanged.RemoveAll(Obj);
	}

	virtual void Initialize_Inner_Implementation() override;
	virtual FText GetDisplayText_Implementation() const override;
};

UCLASS(NotBlueprintable, DisplayName="Zero")
class CRABTOOLSUE5_API UZeroIntAttribute : public UBaseIntAttribute
{
	GENERATED_BODY()


public:

	virtual int GetBaseValue_Implementation() const override { return 0; }
};

UCLASS(NotBlueprintable, DisplayName = "Min")
class CRABTOOLSUE5_API UMinIntAttribute : public UBaseIntAttribute
{
	GENERATED_BODY()


public:

	virtual int GetBaseValue_Implementation() const override { return MIN_int32; }
};

UCLASS(NotBlueprintable, DisplayName = "Max")
class CRABTOOLSUE5_API UMaxIntAttribute : public UBaseIntAttribute
{
	GENERATED_BODY()


public:

	virtual int GetBaseValue_Implementation() const override { return MAX_int32; }
};

UCLASS(Blueprintable, BlueprintType)
class CRABTOOLSUE5_API UIntAttribute : public UBaseIntAttribute
{
	GENERATED_BODY()

private:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RPG|Attributes", meta = (AllowPrivateAccess = true))
	int BaseValue = 0;

public:

	UFUNCTION(BlueprintCallable, Category="RPGProperty")
	void SetBaseValue(int UValue);

protected:

	virtual int GetBaseValue_Implementation() const override { return this->BaseValue; }
	virtual void SetIntegerValue_Implementation(int NewValue) override { this->SetBaseValue(NewValue); }
};

UCLASS(Blueprintable, DefaultToInstanced, CollapseCategories, EditInlineNew)
class CRABTOOLSUE5_API UIntOperator : public UObject
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	int Priority;

	UPROPERTY()
	TArray<TObjectPtr<UBaseIntAttribute>> Applied;

public:

	UFUNCTION(BlueprintNativeEvent, Category = "RPG|Operators")
	int Operate(int Value);
	virtual int Operate_Implementation(int Value) { return Value; }

	UFUNCTION(BlueprintCallable, Category="RPG|Operators")
	void Remove();

	UFUNCTION(BlueprintNativeEvent, Category = "RPG|Operators")
	void Initialize();
	virtual void Initialize_Implementation() {}


	int GetPriority() const { return this->Priority; }

	void AddApplied(UBaseIntAttribute* Attr);
	void RemoveApplied(UBaseIntAttribute* Attr);

	UFUNCTION(BlueprintCallable, Category="RPG|Operators")
	void Refresh();
};

/*
 *  Operation for IntAttributes, Comparisons happen in the order Property OP Value.
 */
UCLASS(Blueprintable)
class UIntegerPropertySetter : public URPGSetter
{
	GENERATED_BODY()

private:

	/* Whether to set the value of the property to the inline Value, or to use a property. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPGProperty",
		meta = (AllowPrivateAccess))
	bool bInline = true;

	/* The inline Value to set the property to. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPGProperty",
		meta = (AllowPrivateAccess, EditCondition="bInline", EditConditionHides))
	int Value = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPGProperty",
		meta = (AllowPrivateAccess, EditCondition = "!bInline", EditConditionHides,
			GetOptions="GetPropertyNames"))
	FName SourcePropertyName;

	UPROPERTY()
	TObjectPtr<URPGProperty> SourceProperty;

public:

	virtual void ApplyValue_Implementation() override;
	virtual UClass* GetPropertySearchType_Implementation() const override
	{
		return UIntegerRPGProperty::StaticClass();
	}

protected:

	virtual void Initialize_Inner_Implementation() override;

};

/*
 *  Operation for IntAttributes, Comparisons happen in the order Property OP Value.
 */
UCLASS(Blueprintable)
class UIntegerPropertyCompare : public URPGCompare
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
	int Value = 0;

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
		return UIntegerRPGProperty::StaticClass();
	}

protected:

	virtual void Initialize_Inner_Implementation() override;

};