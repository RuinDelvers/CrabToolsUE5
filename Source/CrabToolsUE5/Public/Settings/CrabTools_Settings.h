#pragma once

#include "Engine/DeveloperSettings.h"
#include "Types/SlateEnums.h"
#include "CrabTools_Settings.generated.h"

class UStatusDataRepo;

/**
 * 
 */
UCLASS(MinimalAPI, config=Game, DisplayName="CrabTools Settings")
class UCrabTools_Settings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	
	UPROPERTY(Config, EditDefaultsOnly, Category="UI")
	bool bUseCustomUINavigationRules = false;

	UPROPERTY(Config, EditDefaultsOnly, Category = "UI|Navigation",
		meta=(ShowOnlyInnerProperties, ForceInlineRow,
			EditCondition = "bUseCustomUINavigationRules", EditConditionHides,
			ReadOnlyKeys))
	TMap<FKey, EUINavigation> NavControl;

	UPROPERTY(Config, EditDefaultsOnly, Category = "UI|Navigation",
		meta = (ShowOnlyInnerProperties, ForceInlineRow,
			EditCondition="bUseCustomUINavigationRules", EditConditionHides,
			ReadOnlyKeys))
	TMap<FKey, EUINavigationAction> NavActions;

public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Settings")
	static const UCrabTools_Settings* GetSettings();

public:

	UCrabTools_Settings();

	UFUNCTION(BlueprintCallable, Category="UI")
	void ApplyCustomUINavigation() const;

protected:

	virtual void PostInitProperties() override;
};