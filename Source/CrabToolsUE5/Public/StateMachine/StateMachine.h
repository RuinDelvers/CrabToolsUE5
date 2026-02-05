#pragma once

#include "Utils/Enums.h"
#include "UObject/ObjectPtr.h"
#include "StateMachine/EventListener.h"
#include "StateMachine/IStateMachineLike.h"
#include "Containers/List.h"
#include "StateMachine/Emitters/EventEmitter.h"
#include "StateMachine.generated.h"

class UStateNode;
class UStateMachine;
class UNodeTransition;
class UStateMachineBlueprintGeneratedClass;

#define STATEMACHINE_DEBUG_DATA WITH_EDITOR || UE_BUILD_DEBUG

/* Structure used to pass to listeners for when states change.*/
USTRUCT(BlueprintType)
struct FStateChangedEventData
{
	GENERATED_USTRUCT_BODY()

	/* Which state we transitioned from. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="StateMachine")
	FName From;

	/* Which state we have transitioned into. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StateMachine")
	FName To;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StateMachine")
	TObjectPtr<UState> FromState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StateMachine")
	TObjectPtr<UState> ToState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StateMachine")
	TObjectPtr<UStateMachine> StateMachine;
};

UENUM()
enum class EDefaultStateMachineEvents : uint8
{
	ON_MACHINE_ACTIVATE      UMETA(DisplayName="MachineActivated"),
	ON_MACHINE_DEACTIVATE    UMETA(DisplayName = "MachineDeactivated"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStateChangedEvent, const FStateChangedEventData&, Data);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTransitionFinishedEvent, UStateMachine*, Machine);
DECLARE_DYNAMIC_DELEGATE_RetVal(bool, FTransitionDelegate);
DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(bool, FTransitionDataDelegate, UObject*, Data);


/* 
 * Simple interface to use for external modules to relay messages when verifying Node integrity. 
 * This is meant to mimic a KismetCompilerContext somewhat, but exists specifically to avoid the issue
 * of needing an editor only interface in the runtime.
 */
class FNodeVerificationContext
{
private:

	TObjectPtr<UClass> TargetClass;

public:

	FNodeVerificationContext(UClass* Class) : TargetClass(Class) {}

	virtual void Error(FString& Msg, const UObject* Obj) = 0;
	virtual void Warning(FString& Msg, const UObject* Obj) = 0;
	virtual void Note(FString& Msg, const UObject* Obj) = 0;

	UObject* GetOuter() const { return this->TargetClass; }
};

USTRUCT(BlueprintType)
struct FTransitionData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "StateMachine")
	TObjectPtr<UTransitionCondition> Condition;

	UPROPERTY(EditAnywhere, Category = "StateMachine")
	TObjectPtr<UTransitionDataCondition> DataCondition;

public:

	void Validate();
	void Initialize(UStateMachine* Owner, FName ISource, FName IDestination);
	void Copy(UObject* NewOwner);
};

USTRUCT(BlueprintType)
struct FTransitionDataSet
{
	GENERATED_BODY()

public:

	/* Map from destination to their conditions.*/
	UPROPERTY(VisibleAnywhere, Category="Transitions")
	TMap<FName, FTransitionData> Destinations;

	void Initialize(UStateMachine* Owner, FName ISource);
	void Copy(UObject* NewOwner);

};

struct FDestinationResult
{
	FName Destination = NAME_None;
	TObjectPtr<UAbstractCondition> Condition;
};

UCLASS(Blueprintable, CollapseCategories, Category = "StateMachine")
class CRABTOOLSUE5_API UState : public UObject, public IStateLike
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Transient, DuplicateTransient, Category="StateMachine",
		meta=(AllowPrivateAccess))
	TObjectPtr<UStateMachine> OwnerMachine;

	UPROPERTY(VisibleAnywhere, DuplicateTransient, Category = "StateMachine", meta = (AllowPrivateAccess))
	TObjectPtr<UStateNode> Node;

	// Map from Event Name to StateName
	UPROPERTY(VisibleAnywhere, DuplicateTransient, Category = "StateMachine", meta = (AllowPrivateAccess))
	TMap<FName, FTransitionDataSet> Transitions;

public:

	#if WITH_EDITORONLY_DATA
		DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEventCallback, FName, EventName);

		FEventCallback OnEventReceived;

		DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEventWithDataCallback, FName, EventName, UObject*, Data);

		FEventWithDataCallback OnEventWithDataReceived;
	#endif

public:

	/* Only use to build states. Do not use for currently in use States/State Machines */
	void Append(UState* Data);
	/* Only use to build states. Do not use for currently in use States/State Machines */
	void AppendCopy(UState* Data);
	/* Only use to build states. Do not use for currently in use States/State Machines */
	void AppendNode(UStateNode* Node);
	/* Only use to build states. Do not use for currently in use States/State Machines */
	void AppendNodeCopy(UStateNode* Node);

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	UObject* GetOwner() const;

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	AActor* GetActorOwner() const;

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	UStateMachine* GetMachine() const { return this->OwnerMachine; }

	
	//FORCEINLINE const TMap<FName, FTransitionData>& GetTransitions() const { return Transitions; }

	void AddTransition(FName EventName, FName Destination, FTransitionData Data);
	void AddTransition(FName EventName, FTransitionDataSet Data);
	void AddTransition(const TMap<FName, FTransitionDataSet>& Data);

	void Initialize(UStateMachine* POwner);
	void Enter();
	void EnterWithData(UObject* Data);
	void Exit();
	void ExitWithData(UObject* Data);
	void Event(FName InEvent);
	void EventWithData(FName InEvent, UObject* Data);
	void Tick(float DeltaTime);
	bool RequiresTick() const;
	bool HasPipedData() const;
	UObject* GetPipedData() const;

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	UStateNode* GetNode() const { return this->Node; }
	bool HasNode() const;
	void SetNode(UStateNode* InNode) { this->Node = Node; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="StateMachine")
	bool IsActive() const;

	UFUNCTION(BlueprintNativeEvent, Category = "StateMachine")
	void Enter_Inner();
	virtual void Enter_Inner_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, Category = "StateMachine")
	void EnterWithData_Inner(UObject* Data);
	virtual void EnterWithData_Inner_Implementation(UObject* Data) { this->Enter_Inner(); }

	UFUNCTION(BlueprintNativeEvent, Category = "StateMachine")
	void Exit_Inner();
	virtual void Exit_Inner_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, Category = "StateMachine")
	void ExitWithData_Inner(UObject* Data);
	virtual void ExitWithData_Inner_Implementation(UObject* Data) { this->Exit_Inner(); }

	bool GetDestination(FName InEvent, FDestinationResult& Result);
	bool GetDataDestination(FName InEvent, UObject* Data, FDestinationResult& Result);

	void GetTransitionEvents(TSet<FName>& Events) const;

private:

	void EnterConditions();
	void ExitConditions();
	void Event_Inner(FName InEvent) const;
	void EventWithData_Inner(FName InEvent, UObject* Data) const;
};

/*
 * A StateNode can be in multiple states of entering, active, and exiting. These describe which state it
 * is in.
 */
UENUM()
enum class EStateNodeState
{
	/* The state node is being entered. It is considered active. */
	ENTERING UMETA(DisplayName="Entering"),
	/* The state node has been fully entered, Enter has been called for the node tree. Considered Active. */
	ENTERED UMETA(DisplayName = "Entered"),
	/* The node has been entered, but set to be inactive. */
	ENTERED_INACTIVE UMETA(DisplayName = "Entered Inactive"),
	/* The node is exiting. Considered active. */
	EXITING UMETA(DisplayName = "Exiting"),
	/* The node has been exited, and is considered inactive. */
	INACTIVE UMETA(DisplayName = "Inactive"),

};

/**
 * Base Node class for an individual node in a statemachine. Defines the general behaviour and interface
 * for what nodes can do.
 */
UCLASS(Abstract, Blueprintable, EditInlineNew, CollapseCategories, Category = "StateMachine")
class CRABTOOLSUE5_API UStateNode : public UObject, public IStateNodeLike
{
	GENERATED_BODY()

	//friend class UStateMachine;

	UPROPERTY(Transient, DuplicateTransient)
	TObjectPtr<UStateMachine> Owner;

	EStateNodeState CurrentState = EStateNodeState::INACTIVE;

	#if WITH_EDITORONLY_DATA
		UPROPERTY(EditDefaultsOnly, Category="StateMachine|Events",
			meta=(AllowPrivateAccess, HideInDetailPanel))
		TSet<FName> EmittedEvents;
		TSet<FName> PreEditEmittedEvents;
	#endif

	DECLARE_DELEGATE_OneParam(FEventNotify_Single, FName);
	DECLARE_MULTICAST_DELEGATE_OneParam(FEventNotify, FName);
	DECLARE_DELEGATE_TwoParams(FEventWithDataNotify_Single, FName, UObject*);
	DECLARE_MULTICAST_DELEGATE_TwoParams(FEventWithDataNotify, FName, UObject*);

	TMap<FName, FEventNotify> EventNotifies;
	TMap<FName, FEventWithDataNotify> EventWithDataNotifies;

public:

	UStateNode();

	/**
	 * Function used to ensure proper state setup happens. Only call this if you need to
	 * manually initialize a state machine.
	 */
	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	void Initialize(UStateMachine* POwner);

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	void SetOwner(UStateMachine* Parent);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="StateMachine")
	FORCEINLINE bool Active() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateMachine")
	UObject* GetOwner() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateMachine")
	AActor* GetActorOwner() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateMachine")
	UStateMachine* GetMachine() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateMachine")
	UStateMachine* GetRootMachine() const;

	UFUNCTION(BlueprintCallable, Category = "StateMachine", meta = (DeterminesOutputType = "SClass"))
	UStateMachine* GetMachineAs(TSubclassOf<UStateMachine> SClass, bool& bFound) const;

	UFUNCTION(BlueprintCallable, Category = "StateMachine", meta = (DeterminesOutputType = "SClass"))
	UStateMachine* GetRootMachineAs(TSubclassOf<UStateMachine> SClass, bool& bFound) const;

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "StateMachine",
		meta = (HideSelfPin=true))
	void EmitEvent(FName InEvent);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateMachine")
	UState* GetState() const;

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "StateMachine",
		meta = (HideSelfPin=true))
	void EmitEventWithData(FName InEvent, UObject* Data);

	UFUNCTION(BlueprintCallable, Category="State")
	FORCEINLINE EStateNodeState GetNodeState() const { return this->CurrentState; }

	UFUNCTION(BlueprintNativeEvent, Category = "StateMachine")
	bool RequiresTick() const;
	virtual bool RequiresTick_Implementation() const;

	#if WITH_EDITOR
		virtual void GetNotifies(TSet<FName>& Events) const;
		virtual void GetEmittedEvents(TSet<FName>& Events) const;
		virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
		virtual void PreEditChange(FProperty* PropertyAboutToChange) override;
	#endif

		virtual bool Modify(bool bShouldAlwaysMarkDirty = true) override;

	/* Only call from an SM or a managing Node. */
	void Event(FName InEvent);

	/* Only call from an SM or a managing Node. */
	void EventWithData(FName InEvent, UObject* Data);

	/* Only call from an SM or a managing Node. */
	void Enter();

	/* Only call from an SM or a managing Node. */
	void EnterWithData(UObject* Data);

	/* Only call from an SM or a managing Node. */
	void Tick(float DeltaTime);

	/* Only call from an SM or a managing Node. */
	void Exit();

	/* Only call from an SM or a managing Node. */
	void ExitWithData(UObject* Data);

	/* Only call from an SM or a managing Node. */
	void SetActive(bool bNewActive);

	/* Runs a verification check on the node. Returns true if no error, false if an error happened. */
	bool Verify(FNodeVerificationContext& Context) const;

	UFUNCTION(BlueprintNativeEvent, Category = "StateMachine")
	bool HasPipedData() const;
	virtual bool HasPipedData_Implementation() const { return false; }

	UFUNCTION(BlueprintNativeEvent, Category = "StateMachine")
	UObject* GetPipedData();
	virtual UObject* GetPipedData_Implementation() { return nullptr; }

	virtual UWorld* GetWorld() const override;

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	bool DoesReferenceMachine(FName MachineName) const;

protected:

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	void UpdateTickRequirements() const;

	/* Override this with your verification code. */
	virtual bool Verify_Inner(FNodeVerificationContext& Context) const { return true; }

	/* Function called by Initialize_Internal. Override this to setup your init code. */
	UFUNCTION(BlueprintNativeEvent, Category = "StateMachine", meta=(DisplayName="Initialize"))
	void Initialize_Inner();
	virtual void Initialize_Inner_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "StateMachine", meta = (DisplayName = "Event"))
	void Event_Inner(FName InEvent);	
	virtual void Event_Inner_Implementation(FName InEvent);

	UFUNCTION(BlueprintNativeEvent, Category = "StateMachine", meta = (DisplayName = "EventWithData"))
	void EventWithData_Inner(FName InEvent, UObject* Data);
	virtual void EventWithData_Inner_Implementation(FName InEvent, UObject* Data);

	UFUNCTION(BlueprintNativeEvent, Category = "StateMachine", meta = (DisplayName = "Enter"))
	void Enter_Inner();	
	virtual void Enter_Inner_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, Category = "StateMachine", meta = (DisplayName = "EnterWithData"))
	void EnterWithData_Inner(UObject* Data);	
	virtual void EnterWithData_Inner_Implementation(UObject* Data);

	UFUNCTION(BlueprintNativeEvent, Category = "StateMachine", meta = (DisplayName = "Tick"))
	void Tick_Inner(float DeltaTime);	
	virtual void Tick_Inner_Implementation(float DeltaTime) {}

	UFUNCTION(BlueprintNativeEvent, Category = "StateMachine", meta = (DisplayName = "Exit"))
	void Exit_Inner();
	virtual void Exit_Inner_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StateMachine", meta = (DisplayName = "ExitWithData"))
	void ExitWithData_Inner(UObject* Data);	
	virtual void ExitWithData_Inner_Implementation(UObject* Data);

	UFUNCTION(BlueprintNativeEvent, Category = "StateMachine", meta = (DisplayName = "OnSetActive"))
	void SetActive_Inner(bool bNewActive);
	virtual void SetActive_Inner_Implementation(bool bNewActive) {}

	UFUNCTION(BlueprintNativeEvent, Category = "StateMachine", meta = (DisplayName = "DoesReferenceMachine"))
	bool DoesReferenceMachine_Inner(FName MachineName) const;
	virtual bool DoesReferenceMachine_Inner_Implementation(FName MachineName) const { return false; }

	#if WITH_EDITOR
		UFUNCTION()
		TArray<FString> GetEventOptions() const;

		UFUNCTION()
		TArray<FString> GetMachineOptions() const;

		UFUNCTION()
		TArray<FString> GetStateOptions() const;

		UFUNCTION()
		TArray<FString> GetIncomingStateOptions() const;

		UFUNCTION()
		TArray<FString> GetOutgoingStateOptions() const;
	#endif

	void AddEmittedEvent(FName Event)
	{
		#if WITH_EDITORONLY_DATA
			this->EmittedEvents.Add(Event);
		#endif
	}

private:

	UFUNCTION()
	void EventNotifySignatureFunction(FName Name) {}

	UFUNCTION()
	void EventWithDataNotifySignatureFunction(FName Name, UObject* Data) {}

	void InitNotifies();
};

UCLASS(BlueprintType, Abstract, Category = "StateMachine")
class CRABTOOLSUE5_API UAbstractCondition : public UStateNode
{
	GENERATED_BODY()

	UPROPERTY(Transient, DuplicateTransient)
	FName Source;

	UPROPERTY(Transient, DuplicateTransient)
	FName Destination;

public:

	void Initialize(UStateMachine* NewOwner, FName ISource, FName IDestination);

	UFUNCTION(BlueprintNativeEvent, Category = "StateMachine|Transition")
	void OnTransitionTaken();
	virtual void OnTransitionTaken_Implementation() {}

};

UCLASS(BlueprintType, Abstract, Category = "StateMachine")
class CRABTOOLSUE5_API UTransitionCondition : public UAbstractCondition
{
	GENERATED_BODY()

public:


	virtual bool Check() const { return false; }

protected:


};

UCLASS(BlueprintType, Abstract, Category = "StateMachine")
class CRABTOOLSUE5_API UTransitionDataCondition : public UAbstractCondition
{
	GENERATED_BODY()

public:

	virtual bool Check(UObject* Data) const { return false; }

};

USTRUCT()
struct FTransitionQueue
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, Category = "Transition")
	int TransitionID = 0;

	UPROPERTY(VisibleAnywhere, Category="Transition")
	FName Destination;

	UPROPERTY(VisibleAnywhere, Category = "Transition")
	FName CurrentEvent;

	UPROPERTY(VisibleAnywhere, Category = "Transition")
	bool bHasData = false;

	UPROPERTY(VisibleAnywhere, Category = "Transition")
	TObjectPtr<UObject> Data;

	UPROPERTY(VisibleAnywhere, Category = "Transition")
	TObjectPtr<UAbstractCondition> Transition;

public:

	void Queue(FName NewDestination, FName Event = NAME_None, UAbstractCondition* TakenTransition = nullptr);
	void Queue(FName NewDestination, UObject* NewData, FName Event = NAME_None, UAbstractCondition* TakenTransition = nullptr);
	void Clear();
};

USTRUCT()
struct FEventQueueData
{
	GENERATED_BODY()

	UPROPERTY()
	FName EventName;

	UPROPERTY()
	bool bHasData = false;

	UPROPERTY()
	TObjectPtr<UObject> Data;

public:

	FEventQueueData() {}
	FEventQueueData(FName InEvent) : EventName(InEvent) {}
	FEventQueueData(FName InEvent, UObject* InData)
	: EventName(InEvent), bHasData(true), Data(InData)
	{}
};

/**
 * State Machine class. This is the base class for any State Machine, and manages all
 * appropriate state machine behaviour.
 */
UCLASS(Blueprintable, CollapseCategories, EditInlineNew, Category = "StateMachine")
class CRABTOOLSUE5_API UStateMachine 
: public UObject, public IEventListenerInterface, public IStateMachineLike
{
	GENERATED_BODY()

private:
	#if STATEMACHINE_DEBUG_DATA
		/* Whether or not this SM was initialized. Used for debugging only. */
		bool bWasInitialized = false;
	#endif

	enum class TransitionPhase
	{
		None,
		Exiting,
		Entering,
	};

	UPROPERTY(VisibleAnywhere, Category="Transition", meta=(ShowOnlyInnerProperties))
	FTransitionQueue Queue;
	TransitionPhase TransPhase = TransitionPhase::None;
	FStateChangedEventData StateChangedData;

	/* Used when events are received while transitioning. */
	TArray<FEventQueueData> EventQueue;

	/* Simple cached value for retrieving start state data. Do not read from this directly. Use GetStartState. */
	mutable FName CachedStartState;

	/* The Graph of the state machine. */
	UPROPERTY(DuplicateTransient, meta=(IgnorePropertySearch))
	TMap<FName, TObjectPtr<UState>> Graph;

	/* Nodes to be substituted into the graph later. */
	UPROPERTY(EditAnywhere, Instanced, Category = "StateMachine",
		meta = (AllowPrivateAccess, IgnorePropertySearch))
	TMap<FName, TObjectPtr<UStateNode>> SharedNodes;

	/* State Machines to be substituted into the graph later. */
	UPROPERTY(EditAnywhere, Instanced, Category = "StateMachine",
		meta = (AllowPrivateAccess, IgnorePropertySearch))
	TMap<FName, TObjectPtr<UStateNode>> SharedMachines;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "StateMachine",
		meta = (AllowPrivateAccess, IgnorePropertySearch))
	FName CurrentStateName;

	UPROPERTY(EditInstanceOnly, Instanced, Category = "StateMachine",
		meta = (AllowPrivateAccess, IgnorePropertySearch))
	TArray<TObjectPtr<UEventEmitter>> EventEmitters;

	/* How many previous states to remember. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "StateMachine",
		meta = (AllowPrivateAccess, IgnorePropertySearch,
			ClampMin = "2", ClampMax = "1000", UIMin = "2", UIMax = "1000"))
	int MaxPrevStateStackSize = 5;

	/* Sequence of States that this machine has passed through*/
	TDoubleLinkedList<FName> StateStack;

	UPROPERTY(Transient, meta=(IgnorePropertySearch))
	TObjectPtr<UObject> Owner;

	/* Map of name to submachines available to this state machine. */
	UPROPERTY(DuplicateTransient, meta=(IgnorePropertySearch))
	TMap<FName, TObjectPtr<UStateMachine>> SubMachines;

	UPROPERTY(VisibleAnywhere, DuplicateTransient, Category = "StateMachine",
		meta = (AllowPrivateAccess, IgnorePropertySearch))
	float ActiveTime = 0.0f;

	/* Reference to a parent which uses this state machine as a sub machine. */
	TObjectPtr<UStateMachine> ParentMachine;
	/* The key/name of this submachine in the parent. */
	FName ParentKey;

	/*
	 * Whether or not this state machine is active. Inactivate state machines cannot have events
	 * sent to them, and they will not update their states. This is propagated to current nodes
	 * state nodes.
	 */
	bool bIsActive = true;
	
	UPROPERTY()
	TSet<EDefaultStateMachineEvents> ActiveDefaultEvents;

public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTickRequirementUpdated, bool, NeedsTick);

	UPROPERTY(BlueprintAssignable, Category = "StateMachine", meta = (IgnorePropertySearch))
	FTickRequirementUpdated OnTickRequirementUpdated;

	UPROPERTY(BlueprintAssignable, Category="StateMachine", meta = (IgnorePropertySearch))
	FStateChangedEvent OnStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "StateMachine", meta = (IgnorePropertySearch))
	FTransitionFinishedEvent OnTransitionFinished;

public:

	UStateMachine(const FObjectInitializer& ObjectInitializer);

	const TArray<UEventEmitter*>& GetEmitters() const { return this->EventEmitters; }

	/**
	 * Function used to ensure proper state setup happens. Only call this if you need to manually initialize a
	 * state machine.
	 */
	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	void Initialize(UObject* POwner);

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	void SetActive(bool bNewActive);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateMachine")
	AActor* GetActorOwner() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateMachine")
	UObject* GetOwner() const;

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	void Reset();

	/*
	 * Current Event that is being processed for Exit/Enter/Event calls.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateMachine")
	FName GetCurrentEvent() const { return this->Queue.CurrentEvent; }

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "StateMachine")
	void SendEvent(FName InEvent);
	virtual void Event_Implementation(FName InEvent) override final { this->SendEvent(InEvent); }

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "StateMachine")
	void SendEventWithData(FName InEvent, UObject* Data);
	void EventWithData_Implementation(FName InEvent, UObject* Data) override final { this->SendEventWithData(InEvent, Data); }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateMachine",
		meta = (ExpandEnumAsExecs = "Branches"))
	UStateNode* GetCurrentStateAs(TSubclassOf<UStateNode> Class, ESearchResult& Branches);

	void Tick(float DeltaTime);

	/* This function returns the current state if it is loaded, null if it is not.*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateMachine")
	UState* GetCurrentStateData() const;

	/*
	 * This function returns the current state. It will load in the current state from any
	 * State Machine Blueprints that are necessary.
	 */
	UState* GetCurrentState();
	UState* GetStateData(FName Name);

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	float GetActiveTime() const { return this->ActiveTime; }

	UFUNCTION(BlueprintCallable, Category="StateMachine")
	bool IsActiveState(const UState* State) const;

	UFUNCTION(BlueprintCallable, Category="StateMachine")
	FORCEINLINE bool Active() const { return this->bIsActive; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateMachine")
	FName GetCurrentStateName() const;

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	UStateMachine* GetParentMachine() const { return this->ParentMachine; }
	void SetParentMachine(UStateMachine* Machine) { this->ParentMachine = Machine; }

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	TArray<FString> StateOptions();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="StateMachine")
	UStateMachine* GetRootMachine();

	UFUNCTION()
	TArray<FString> ConditionOptions() const;

	UFUNCTION()
	TArray<FString> ConditionDataOptions() const;

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	FName GetPreviousStateName() const;

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	UState* GetPreviousState() const;

	/* Condition function that always returns true. */
	UFUNCTION(meta = (IsDefaultCondition))
	bool TrueCondition();

	UFUNCTION(meta = (IsDefaultCondition))
	bool FalseCondition();

	/* Condition function that always returns true. */
	UFUNCTION(meta = (IsDefaultCondition))
	bool TrueDataCondition(UObject* Data);

	UFUNCTION(meta = (IsDefaultCondition))
	bool FalseDataCondition(UObject* Data);

	/* Condition function that returns true if Data is Valid. */
	UFUNCTION(meta = (IsDefaultCondition))
	bool ValidDataCondition(UObject* Data);

	TSet<FName> GetEvents() const;
	TMap<FName, TObjectPtr<UStateNode>> GetSharedNodes() { return this->SharedNodes; }

	void SetParentData(UStateMachine* Parent, FName NewParentKey);
	UStateMachineBlueprintGeneratedClass* GetGeneratedClass() const;

	#if WITH_EDITOR
		virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
		virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent) override;
		virtual void PreEditChange(FProperty* PropertyAboutToChange) override;
		virtual void PostLinkerChange() override;

		/* This is an editor only function used for compiling state machines.*/
		void AppendDefaultEvents(const TSet<EDefaultStateMachineEvents>& Events);
	#endif

	// IStateMachineLike interface
	virtual TArray<FString> GetStateOptions(const UObject* Asker) const override;
	virtual IStateMachineLike* GetSubMachine(FString& Address) const override;

	/* Returns the submachine defined by this slot. */
	UFUNCTION(BlueprintCallable, Category="SubMachine")
	UStateMachine* GetSubMachine(const FSubMachineSlot& Slot) const;

	/* Used to bind transition delegates. */
	void BindConditionAt(FString& Address, FTransitionDelegate& Condition);
	/* Used to bind transition delegates. */
	void BindDataConditionAt(FString& Address, FTransitionDataDelegate& Condition);

	UState* DuplicateStateObject(FName StateName, UObject* NewOuter) const;

	// Procedural construction functions.
	UState* MakeState(FName StateName);
	UState* MakeStateWithNode(FName StateName, UStateNode* Node);
	void AddStateData(FName StateName, UState* Data);
	// End Procedural Construction functions

	void UpdateTickRequirements(bool NeedsTick);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateMachine", meta=(DeterminesOutputType="MachineClass"))
	UStateMachine* FindMachineByClass(TSubclassOf<UStateMachine> MachineClass);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateMachine", meta = (DeterminesOutputType = "MachineClass"))
	UStateMachine* FindMachineByInterface(TSubclassOf<UInterface> MachineClass);

	virtual UWorld* GetWorld() const override;

	void AddEventEmitter(UEventEmitter* Emitter) { this->EventEmitters.Add(Emitter); }

protected:

	FName GetStartState() const;

	UFUNCTION(BlueprintNativeEvent, Category = "StateMachine")
	void Initialize_Inner();
	virtual void Initialize_Inner_Implementation();

private:

	#if STATEMACHINE_DEBUG_DATA
		void NotInitializedError();
	#endif // STATEMACHINE_DEBUG_DATA

	void InitFromArchetype();
	void PushStateToStack(FName InEvent);
	void UpdateState();
	void InitSubMachines();

	/* Helpers for UpdateState */
	void UpdateTransitionPipedData(UState* CurrentState, FTransitionQueue& Cached);
	void SetupStateChangedData();
	bool DidPreempt(const FTransitionQueue& Cached) const;
	void GetPreemptTransition(FTransitionQueue& Cached) const;
	/* End Helpers for UpdateState */
};
