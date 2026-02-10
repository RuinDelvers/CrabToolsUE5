#pragma once

#include "Utils/UtilsLibrary.h"
#include "StateMachine/StateMachine.h"
#include "Components/RPGSystem/RPGProperty.h"
#include "SetRPGPropertyNode.generated.h"

class URPGComponent;
class URPGSetter;

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

	UPROPERTY(EditAnywhere, Category = "RPG")
	ERPGSetterOperation OnEnter = ERPGSetterOperation::NONE;

	UPROPERTY(EditAnywhere, Category = "RPG")
	ERPGSetterOperation OnExit = ERPGSetterOperation::NONE;

	UPROPERTY(EditAnywhere, Category = "RPG")
	ERPGSetterOperation OnPostTransition = ERPGSetterOperation::NONE;

protected:

	virtual void Initialize_Inner_Implementation() override;
	virtual void Enter_Inner_Implementation() override;
	virtual void Exit_Inner_Implementation() override;
	virtual void PostTransition_Implementation() override;

	#if WITH_EDITOR
		virtual void PostEditChangeProperty(FPropertyChangedEvent& Event) override;

		UFUNCTION()
		TArray<FString> GetPropertyOptions() const;

		void UpdateSetterObject();
	#endif

private:

	void ApplyAction(ERPGSetterOperation Op) const;
};