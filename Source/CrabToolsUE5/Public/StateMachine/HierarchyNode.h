#pragma once

#include "StateMachine/StateMachine.h"
#include "StateMachine/DataStructures.h"
#include "HierarchyNode.generated.h"

USTRUCT(BlueprintType)
struct FHierarchyEventValue
{
	GENERATED_BODY()

	/* Use custom inlined event? */
	UPROPERTY(EditDefaultsOnly, Category = "StateMachine")
	EHierarchyInputType EventType = EHierarchyInputType::INLINED;

	UPROPERTY(EditDefaultsOnly, Category = "Events",
		meta = (EditCondition = "EventType == EHierarchyInputType::INLINED", EditConditionHides))
	FName InlinedEvent;

	UPROPERTY(EditDefaultsOnly, Category = "Events",
		meta = (EditCondition = "EventType == EHierarchyInputType::DEFINED", EditConditionHides,
			GetOptions = "GetStateEventOptions"))
	FName DefinedEvent;

	FName GetEvent() const;
};

/**
 * State Machine node used that is controlled by a StateMachine.
 */
UCLASS(Category = "StateMachine")
class CRABTOOLSUE5_API UHierarchyNode : public UStateNode
{
	GENERATED_BODY()
	
	/* Name of the submachine to be placed in this node. */
	UPROPERTY(EditAnywhere, Category="StateMachine",
		meta=(ShowOnlyInnerProperties))
	FSubMachineSlot Slot;

	/* Map of SubMachine states and events to be emitted to the SM of this node. */
	UPROPERTY(EditAnywhere, Category = "StateMachine", 
		meta = (AllowPrivateAccess = "true", 
			GetKeyOptions="GetSubMachineStateOptions", ForceInlineRow, ShowOnlyInnerProperties))
	TMap<FName, FHierarchyEventValue> ExitStates;

	/* Whether or not the submachine should be reset when this node is entered. */
	UPROPERTY(EditAnywhere, Category = "StateMachine", meta = (AllowPrivateAccess = "true"))
	bool ResetOnEnter = true;

	/*
	 * The event to pass to the submachine when entering. Useful for when ResetOnEnter is false, but
	 * work needs to be continued. Specifically, if ResetOnEnter is false, and nothing is done to transition
	 * the state, on the next tick or event the exit state will be detected again.
	 */
	UPROPERTY(EditAnywhere, Category = "StateMachine",
		meta = (AllowPrivateAccess = "true", GetOptions = "GetSubMachineTransitionEvents",
			EditCondition = "!ResetOnEnter", EditConditionHides))
	FName EnterEventName;

	/* Whether or not the submachine should be reset when this node is exited. */
	UPROPERTY(EditAnywhere, Category = "StateMachine", meta = (AllowPrivateAccess = "true"))
	bool ResetOnExit = true;

	/* Event used when exiting this node. Will pass this event to the submachine upon exiting.*/
	UPROPERTY(EditAnywhere, Category = "StateMachine",
		meta = (AllowPrivateAccess = "true", GetOptions = "GetSubMachineTransitionEvents",
			EditCondition = "!ResetOnExit", EditConditionHides))
	FName ExitEventName;

	/* Whether or not to pass the event which activated this state to the submachine. */
	UPROPERTY(EditAnywhere, Category = "StateMachine", meta = (AllowPrivateAccess = "true"))
	bool bPropagateEnterEvent = false;

	/* If this is true, then the child machine will never be turned inactive. */
	UPROPERTY(EditAnywhere, Category = "StateMachine", meta = (AllowPrivateAccess = "true"))
	bool bMachineAlwaysActive = false;

public:

	virtual void Initialize_Inner_Implementation() override;
	virtual void Event_Inner_Implementation(FName InEvent) override;
	virtual void EventWithData_Inner_Implementation(FName InEvent, UObject* Data) override;
	virtual void Enter_Inner_Implementation() override;
	virtual void Tick_Inner_Implementation(float DeltaTime) override;
	virtual void Exit_Inner_Implementation() override;
	virtual bool RequiresTick_Implementation() const override;
	virtual void SetActive_Inner_Implementation(bool bNewActive) override;

	virtual bool DoesReferenceMachine_Inner_Implementation(FName MachineName) const override;

	#if WITH_EDITOR
		virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
		virtual void GetEmittedEvents(TSet<FName>& Events) const;
	#endif

private:

	UFUNCTION()
	void StateChangedCallback(UStateMachine* Data);

	/* Handle checking for submachine states, and if in appropriate state, pass an event to Owner Machine. */
	void PerformExit();

	#if WITH_EDITOR
		UFUNCTION()
		TArray<FString> GetSubMachineStateOptions() const;

		UFUNCTION()
		TArray<FString> GetSubMachineTransitionEvents() const;

		UFUNCTION()
		TArray<FString> GetStateEventOptions() const;
	#endif
};
