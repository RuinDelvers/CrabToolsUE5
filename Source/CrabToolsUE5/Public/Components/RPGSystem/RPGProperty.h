#pragma once

#include "CoreMinimal.h"
#include "RPGProperty.generated.h"

class URPGComponent;

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

protected:

	virtual void Initialize_Inner_Implementation();
};