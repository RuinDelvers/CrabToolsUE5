#pragma once

#include "Components/ProgressBar.h"
#include "RPGResourceProgressBar.generated.h"

class URPGProperty;
class URPGResource;

UCLASS()
class URPGResourceProgressBar : public UProgressBar
{
	GENERATED_BODY()

private:

	UPROPERTY()
	TObjectPtr<URPGResource> Property;

public:

	UFUNCTION(BlueprintCallable, Category="RPG|Properties")
	void SetProperty(URPGResource* Prop);

private:

	UFUNCTION()
	void OnPropertyChanged(URPGProperty* Prop);
};