#pragma once

#include "Components/RPGSystem/RPGProperty.h"
#include "Components/RPGSystem/Properties/FloatAttribute.h"
#include "FloatResource.generated.h"


UCLASS(Blueprintable, BlueprintType)
class CRABTOOLSUE5_API UFloatResource : public URPGResource
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, Instanced, Category = "RPGProperty")
	TObjectPtr<UBaseFloatAttribute> Minimum;

	UPROPERTY(EditAnywhere, Instanced, Category = "RPGProperty")
	TObjectPtr<UBaseFloatAttribute> Maximum;

	UPROPERTY(EditAnywhere, Category = "RPGProperty")
	float Value = 0;

public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FResourceChanged, UFloatResource*, Resource);

	UPROPERTY(BlueprintAssignable, Category = "RPGProperty")
	FResourceChanged OnResourceChanged;

public:

	UFloatResource();

	UFUNCTION(BlueprintCallable, Category = "RPGProperty",
		meta = (CompactNodeTitle = "->", BlueprintAutocast))
	float GetValue() const { return this->Value; }

	UFUNCTION(BlueprintCallable, Category = "RPGProperty")
	void SetValue(float UValue);

	UFUNCTION(BlueprintCallable, Category = "RPGProperty")
	float GetMin() const { return this->Minimum->GetValue(); }

	UFUNCTION(BlueprintCallable, Category = "RPGProperty")
	float GetMax() const { return this->Maximum->GetValue(); }

	UFUNCTION(BlueprintCallable, Category = "RPGProperty")
	float GetPercent() const;

	UFUNCTION(BlueprintCallable, Category = "RPGProperty")
	void Refresh();

	virtual void SetMinProp(URPGProperty* Prop) override;
	virtual void SetMaxProp(URPGProperty* Prop) override;

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
	void OnAttributeChanged(UBaseFloatAttribute* Attr);

	UFUNCTION()
	TArray<FString> GetAttributeOptions() const;
};