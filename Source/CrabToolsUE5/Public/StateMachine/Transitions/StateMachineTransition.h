#pragma once

#include "StateMachine/Transitions/BaseTransitions.h"
#include "StateMachine/DataStructures.h"
#include "StateMachineTransition.generated.h"

/**
 *
 */
UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced, CollapseCategories, DisplayName = "StateMachine")
class CRABTOOLSUE5_API UStateMachineTransitionCondition : public UTransitionCondition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Transition")
	FSubMachineSlot Slot;

	UPROPERTY(EditAnywhere, Category="Transition")
	TSet<FName> ValidStates;

	UPROPERTY(EditAnywhere, Category = "Transition")
	bool bResetOnExit = true;

public:

	virtual void Tick_Inner_Implementation(float DeltaTime) override;
	virtual bool Check() const override;
	virtual void Initialize_Inner_Implementation() override;
	virtual void Exit_Inner_Implementation() override;
	virtual void Event_Inner_Implementation(FName InEvent) override;
	virtual void EventWithData_Inner_Implementation(FName InEvent, UObject* Data) override;

private:

};

/**
 *
 */
UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced, CollapseCategories, DisplayName = "StateMachine")
class CRABTOOLSUE5_API UStateMachineTransitionDataCondition : public UTransitionDataCondition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Transition")
	FSubMachineSlot Slot;

	UPROPERTY(EditAnywhere, Category = "Transition")
	TSet<FName> ValidStates;

	UPROPERTY(EditAnywhere, Category = "Transition")
	bool bResetOnExit = true;

public:

	virtual void Tick_Inner_Implementation(float DeltaTime) override;
	virtual bool Check(UObject* Data) const override;
	virtual void Initialize_Inner_Implementation() override;
	virtual void Exit_Inner_Implementation() override;
	virtual void Event_Inner_Implementation(FName InEvent) override;
	virtual void EventWithData_Inner_Implementation(FName InEvent, UObject* Data) override;

private:

};