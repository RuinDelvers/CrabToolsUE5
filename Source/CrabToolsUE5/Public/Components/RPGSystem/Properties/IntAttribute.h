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

	virtual void Initialize_Inner_Implementation() override;

	UFUNCTION(BlueprintCallable, Category = "RPGProperty")
	void Operate(UIntOperator* Op);

	UFUNCTION(BlueprintCallable, Category = "RPGProperty")
	void UnOperate(UIntOperator* Op);

	UFUNCTION(BlueprintCallable, Category = "RPGProperty")
	void Refresh();
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