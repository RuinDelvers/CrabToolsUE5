#pragma once

#include "CoreMinimal.h"
#include "RPGProperty.generated.h"

class URPGComponent;

DECLARE_DYNAMIC_DELEGATE_OneParam(FRPGPropertyChanged, URPGProperty*, Prop);

/* Base class for all RPG type properties.*/
UCLASS(Abstract, NotBlueprintType, Blueprintable, DefaultToInstanced, EditInlineNew)
class URPGProperty : public UObject
{
	GENERATED_BODY()

private:

	UPROPERTY(Transient)
	TObjectPtr<URPGComponent> Owner;

public:

	void Initialize(URPGComponent* Component);

	UFUNCTION(BlueprintCallable, Category="RPGProperty")
	URPGComponent* GetOwner() const { return this->Owner; }

	UFUNCTION(BlueprintNativeEvent, Category="RPGProperty")
	void Initialize_Inner();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "RPGProperty")
	FText GetDisplayText() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "RPGProperty")
	void ListenToProperty(const FRPGPropertyChanged& Callback);
	virtual void ListenToProperty_Implementation(const FRPGPropertyChanged& Callback) {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "RPGProperty")
	void StopListeningToProperty(UObject* Obj);
	virtual void StopListeningToProperty_Implementation(UObject* Obj) {}

protected:

	virtual FText GetDisplayText_Implementation() const;
	virtual void Initialize_Inner_Implementation();
};