#pragma once

#include "StateMachine/StateMachineInterface.h"
#include "StateMachine/StateMachine.h"
#include "StateMachine/DataStructures.h"
#include "HierarchyNode.generated.h"

UENUM(BlueprintType)
enum class EHierarchyInputType : uint8
{
	/* Use an event or state machine that is defined in the blueprint. */
	DEFINED       UMETA(DisplayName = "Defined"),
	/* Use an event or state machine that is inlined in this slot. */
	INLINED       UMETA(DisplayName = "Inlined"),
};

USTRUCT(BlueprintType)
struct FHierarchyEventValue
{
	GENERATED_USTRUCT_BODY()

	/* Use custom inlined event? */
	UPROPERTY(EditDefaultsOnly, Category = "StateMachine")
	EHierarchyInputType EventType = EHierarchyInputType::INLINED;

	UPROPERTY(EditDefaultsOnly, Category="Events",
		meta=(EditCondition="EventType == EHierarchyInputType::INLINED", EditConditionHides))
	FName InlinedEvent;

	UPROPERTY(EditDefaultsOnly, Category = "Events",
		meta = (EditCondition = "EventType == EHierarchyInputType::DEFINED", EditConditionHides,
			GetOptions="GetStateEventOptions"))
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
	
	UPROPERTY(EditDefaultsOnly, Category="StateMachine")
	EHierarchyInputType StateMachineSource = EHierarchyInputType::DEFINED;

	UPROPERTY(EditAnywhere, Instanced, Category = "StateMachine", 
		meta = (AllowPrivateAccess = "true", ShowInnerProperties, ShowOnlyInnerProperties,
			EditCondition = "StateMachineSource == EHierarchyInputType::INLINED", EditConditionHides))
	TObjectPtr<UStateMachine> SubMachine;

	/* Name of the submachine to be placed in this node. */
	UPROPERTY(EditAnywhere, Category="StateMachine",
		meta=(EditCondition= "StateMachineSource == EHierarchyInputType::DEFINED", EditConditionHides,
			ShowOnlyInnerProperties))
	FSubMachineSlot Slot;

	/* Map of SubMachine states and events to be emitted to the SM of this node. */
	UPROPERTY(EditAnywhere, Category = "StateMachine", 
		meta = (AllowPrivateAccess = "true", 
			GetKeyOptions="GetSubMachineStateOptions", ForceInlineRow, ShowOnlyInnerProperties))
	TMap<FName, FHierarchyEventValue> ExitStates;

	/* Whether or not the submachine should be reset when this node is entered. */
	UPROPERTY(EditAnywhere, Category = "StateMachine", meta = (AllowPrivateAccess = "true"))
	bool ResetOnEnter = true;

	/* Whether or not the submachine should be reset when this node is exited. */
	UPROPERTY(EditAnywhere, Category = "StateMachine", meta = (AllowPrivateAccess = "true"))
	bool ResetOnExit = true;

	/* Whether or not to pass the event which activated this state to the submachine. */
	UPROPERTY(EditAnywhere, Category = "StateMachine", meta = (AllowPrivateAccess = "true"))
	bool bPropagateEnterEvent = false;

	/*
	 * The event to pass to the submachine when entering. Useful for when ResetOnEnter is false, but
	 * work needs to be continued. Specifically, if ResetOnEnter is false, and nothing is done to transition 
	 * the state, on the next tick or event the exist state will be detected again.
	 */
	UPROPERTY(EditAnywhere, Category = "StateMachine",
		meta = (AllowPrivateAccess = "true", GetOptions="GetSubMachineTransitionEvents"))
	FName EnterEventName;

	/* Event used when exiting this node. Will pass this event to the submachine upon exiting.*/
	UPROPERTY(EditAnywhere, Category = "StateMachine",
		meta = (AllowPrivateAccess = "true", GetOptions = "GetSubMachineTransitionEvents"))
	FName ExitEventName;

public:

	virtual void Initialize_Inner_Implementation() override;
	virtual void Event_Inner_Implementation(FName EName) override;
	virtual void EventWithData_Inner_Implementation(FName EName, UObject* Data) override;
	virtual void Enter_Inner_Implementation() override;
	virtual void Tick_Inner_Implementation(float DeltaTime) override;
	virtual void Exit_Inner_Implementation() override;
	virtual void PostTransition_Inner_Implementation() override;
	virtual bool RequiresTick_Implementation() const override;

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
