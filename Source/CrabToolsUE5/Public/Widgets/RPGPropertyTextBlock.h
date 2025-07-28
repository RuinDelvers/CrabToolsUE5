#pragma once

#include "Components/TextBlock.h"
#include "RPGPropertyTextBlock.generated.h"

class URPGProperty;

UCLASS()
class URPGPropertyTextBlock : public UTextBlock
{
	GENERATED_BODY()

private:

	UPROPERTY()
	TObjectPtr<URPGProperty> Property;

public:

	UFUNCTION(BlueprintCallable, Category="RPG|Properties")
	void SetProperty(URPGProperty* Prop);

private:

	UFUNCTION()
	void OnPropertyChanged(URPGProperty* Prop);
};