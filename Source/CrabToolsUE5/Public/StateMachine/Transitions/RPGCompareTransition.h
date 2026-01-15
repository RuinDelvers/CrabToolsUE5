#pragma once

#include "StateMachine/StateMachine.h"
#include "RPGCompareTransition.generated.h"

class URPGCompare;
class URPGComponent;

/**
 *
 */
UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced, DisplayName = "Compare RPG Value")
class CRABTOOLSUE5_API URPGCompareTransitionCondition : public UTransitionCondition
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, Category = "RPG")
	TSoftClassPtr<URPGComponent> RPGClass;

	UPROPERTY(EditAnywhere, Category = "RPG", meta = (GetOptions = "GetPropertyOptions",
		EditCondition = "RPGClass", EditConditionHides))
	FName PropertyName;

	UPROPERTY(VisibleAnywhere, Category = "RPG", meta = (ShowInnerProperties))
	TObjectPtr<URPGCompare> Compare;

public:

	virtual bool Check() const override;

protected:

	virtual void Initialize_Inner_Implementation() override;

	#if WITH_EDITOR
		virtual void PostEditChangeProperty(FPropertyChangedEvent& Event) override;

		UFUNCTION()
		TArray<FString> GetPropertyOptions() const;

		void UpdateCompareObject();
	#endif
};

