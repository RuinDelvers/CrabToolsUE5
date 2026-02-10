#pragma once

#include "Components/RPGSystem/RPGProperty.h"
#include "SemaphoreProperty.generated.h"

class USemaphorePropertyCompare;
class USemaphorePropertySetter;

UCLASS(Blueprintable, BlueprintType)
class CRABTOOLSUE5_API USemaphoreProperty : public URPGProperty
{
	GENERATED_BODY()

private:

	UPROPERTY(VisibleAnywhere, Category = "RPGProperty")
	TSet<TObjectPtr<UObject>> SemaphoreLocks;

public:

	UFUNCTION(BlueprintCallable, Category="RPGProperty")
	void AddLock(UObject* NewLock);

	UFUNCTION(BlueprintCallable, Category = "RPGProperty")
	void RemoveLock(UObject* NewLock);

	UFUNCTION(BlueprintCallable, Category="RPGProperty")
	bool LockValue() const;

protected:

	virtual TSubclassOf<URPGSetter> GetSetter_Implementation() const override;
	virtual TSubclassOf<URPGCompare> GetCompare_Implementation() const override;
};

UCLASS(Blueprintable)
class USemaphorePropertySetter : public URPGSetter
{
	GENERATED_BODY()

private:

public:

	virtual void ApplyValue_Implementation() override;
	virtual void UnapplyValue_Implementation() override;
	virtual UClass* GetPropertySearchType_Implementation() const override
	{
		return USemaphoreProperty::StaticClass();
	}
};

/*
 * 
 */
UCLASS(Blueprintable)
class USemaphorePropertyCompare : public URPGCompare
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPGProperty",
		meta = (AllowPrivateAccess, GetOptions = "GetPropertyNames"))
	FName ComparePropertyName;

	UPROPERTY()
	TObjectPtr<URPGProperty> CompareProperty;

public:

	virtual bool Compare_Implementation() override;
	virtual UClass* GetPropertySearchType_Implementation() const override
	{
		return USemaphoreProperty::StaticClass();
	}

protected:

	virtual void Initialize_Inner_Implementation() override;

};