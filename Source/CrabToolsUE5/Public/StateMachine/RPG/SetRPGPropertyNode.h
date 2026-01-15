#pragma once

#include "StateMachine/StateMachine.h"
#include "SetRPGPropertyNode.generated.h"

class URPGComponent;

/**
 * State Machine Node that is a composite of other nodes.
 */
UCLASS(Blueprintable, CollapseCategories, Category = "StateMachine",
	DisplayName="SetRPGProperty")
class CRABTOOLSUE5_API USetRPGPropertyNode : public UStateNode
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, Category="RPG")
	TSoftClassPtr<URPGComponent> RPGClass;

	UPROPERTY(EditAnywhere, Category = "RPG", meta=(GetOptions="GetPropertyOptions",
		EditCondition="RPGClass", EditConditionHides))
	FName PropertyName;

	UPROPERTY(VisibleAnywhere, Category="RPG", meta=(ShowInnerProperties))
	TObjectPtr<URPGSetter> Setter;

	/* Whether to set on Enter (true) or on exit (false). */
	UPROPERTY(EditAnywhere, Category = "RPG")
	bool bSetOnEnterOrExit = true;

protected:

	virtual void Initialize_Inner_Implementation() override;
	virtual void Enter_Inner_Implementation() override;
	virtual void Exit_Inner_Implementation() override;

	#if WITH_EDITOR
		virtual void PostEditChangeProperty(FPropertyChangedEvent& Event) override;

		UFUNCTION()
		TArray<FString> GetPropertyOptions() const;

		void UpdateSetterObject();
	#endif
};