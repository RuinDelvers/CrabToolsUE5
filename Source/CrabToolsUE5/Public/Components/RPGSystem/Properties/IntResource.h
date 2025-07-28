#pragma once

#include "Components/RPGSystem/RPGProperty.h"
#include "Components/RPGSystem/Properties/IntAttribute.h"
#include "IntResource.generated.h"


UCLASS(Blueprintable, BlueprintType)
class CRABTOOLSUE5_API UIntResource : public URPGProperty
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, Instanced, Category="RPGProperty")
	TObjectPtr<UBaseIntAttribute> Minimum;

	UPROPERTY(EditAnywhere, Category = "RPGProperty",
		meta=(GetOptions="GetAttributeOptions"))
	FName MinimumRef;

	UPROPERTY(EditAnywhere, Instanced, Category = "RPGProperty")
	TObjectPtr<UBaseIntAttribute> Maximum;

	UPROPERTY(EditAnywhere, Category = "RPGProperty",
		meta = (GetOptions = "GetAttributeOptions"))
	FName MaximumRef;

	UPROPERTY(EditAnywhere, Category="RPGProperty")
	int Value = 0;

public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FResourceChanged, UIntResource*, Resource);

	UPROPERTY(BlueprintAssignable, Category="RPGProperty")
	FResourceChanged OnResourceChanged;

public:

	UIntResource();

	UFUNCTION(BlueprintCallable, Category="RPGProperty",
		meta = (CompactNodeTitle = "->", BlueprintAutocast))
	int GetValue() const { return this->Value; }

	UFUNCTION(BlueprintCallable, Category = "RPGProperty")
	void SetValue(int UValue);

	UFUNCTION(BlueprintCallable, Category = "RPGProperty")
	int GetMin() const { return this->Minimum->GetValue(); }

	UFUNCTION(BlueprintCallable, Category = "RPGProperty")
	int GetMax() const { return this->Maximum->GetValue(); }

	UFUNCTION(BlueprintCallable, Category = "RPGProperty")
	float GetPercent() const;

	UFUNCTION(BlueprintCallable, Category = "RPGProperty")
	void Refresh();

protected:

	virtual FText GetDisplayText_Implementation() const override;
	virtual void Initialize_Inner_Implementation() override;

	virtual void ListenToProperty_Implementation(const FRPGPropertyChanged& Callback) override
	{
		OnResourceChanged.Add(Callback);
	}

	virtual void StopListeningToProperty_Implementation(UObject* Obj) override
	{
		OnResourceChanged.RemoveAll(Obj);
	}

private:

	UFUNCTION()
	void OnAttributeChanged(UBaseIntAttribute* Attr);

	UFUNCTION()
	TArray<FString> GetAttributeOptions() const;
};