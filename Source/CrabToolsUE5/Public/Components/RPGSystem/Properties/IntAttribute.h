#pragma once

//#include <limits>
#include "Components/RPGSystem/RPGProperty.h"
#include "IntAttribute.generated.h"

class UIntOperator;

UCLASS(Abstract, NotBlueprintType)
class CRABTOOLSUE5_API UBaseIntAttribute: public URPGProperty
{
	GENERATED_BODY()

private:

protected:

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "RPG|Attributes", meta = (AllowPrivateAccess = true))
	TArray<TObjectPtr<UIntOperator>> Operators;

	UPROPERTY()
	int CompValue;

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

	virtual int GetBaseValue_Implementation() const override { return this->BaseValue; }
};

UCLASS(Blueprintable, DefaultToInstanced, CollapseCategories, EditInlineNew)
class CRABTOOLSUE5_API UIntOperator : public UObject
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	int Priority;

	URPGComponent* Owner;

public:

	UFUNCTION(BlueprintNativeEvent, Category = "RPG|Operators")
	int Operate(int Value);
	virtual int Operate_Implementation(int Value) { return Value; }

	UFUNCTION(BlueprintNativeEvent, Category = "RPG|Operators")
	void Initialize();
	virtual void Initialize_Implementation() {}


	int GetPriority() const { return this->Priority; }

	void SetOwner(URPGComponent* UOwner) { this->Owner = UOwner; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RPG|Operators")
	URPGComponent* GetOwner() const { return this->Owner; }
};