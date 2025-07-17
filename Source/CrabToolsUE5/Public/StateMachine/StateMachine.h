#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Delegates/Delegate.h"
#include "Templates/UniquePtr.h"
#include "StateChangeListener.h"
#include "Utils/Enums.h"
#include "UObject/ObjectPtr.h"
#include "StateMachine/EventListener.h"
#include "StateMachine/IStateMachineLike.h"
#include "StateMachine/StateMachineEnum.h"
#include "Containers/List.h"
#include "StateMachine/Emitters/EventEmitter.h"
#include "StateMachine.generated.h"

class UStateNode;
class UStateMachine;
class UNodeTransition;
class UStateMachineBlueprintGeneratedClass;

#define STATEMACHINE_DEBUG_DATA WITH_EDITOR || UE_BUILD_DEBUG

#if STATEMACHINE_DEBUG_DATA
	struct FStateMachineDebugDataFrame
	{
		FName Event;
		/* The game-world time in seconds. */
		float Time = 0.0f;
		FName StartState;
		FName EndState;

		bool DidTransition();
	};

	struct FStateMachineDebugDataStack
	{
		TArray<FStateMachineDebugDataFrame> DataFrames;

		// The time the current state was started.
		float CurrentStateTime = -1.0;
	};
#endif

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
	UPROPERTY(EditAnywhere, Category = "StateMachine", meta = (GetOptions = "StateOptions"))
	FName Destination;

	UPROPERTY(EditAnywhere, Category = "StateMachine")
	TObjectPtr<UTransitionCondition> Condition;

	UPROPERTY(EditAnywhere, Category = "StateMachine")
	TObjectPtr<UTransitionDataCondition> DataCondition;
};

UCLASS(Blueprintable, CollapseCategories, Category = "StateMachine")
class CRABTOOLSUE5_API UState : public UObject, public IStateLike
{
	GENERATED_BODY()

	friend class UStateMachine;

	UPROPERTY(VisibleAnywhere, Transient, DuplicateTransient, Category="StateMachine",
		meta=(AllowPrivateAccess))
	TObjectPtr<UStateMachine> OwnerMachine;

	UPROPERTY(VisibleAnywhere, DuplicateTransient, Category = "StateMachine", meta = (AllowPrivateAccess))
	TObjectPtr<UStateNode> Node;

	// Map from Event Name to StateName
	UPROPERTY(VisibleAnywhere, DuplicateTransient, Category = "StateMachine", meta = (AllowPrivateAccess))
	TMap<FName, FTransitionData> Transitions;

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

	UFUNCTION(BlueprintCallable, Category="StateMachine")
	FORCEINLINE UStateNode* GetNode() const { return this->Node; }
	FORCEINLINE const TMap<FName, FTransitionData>& GetTransitions() const { return Transitions; }

	void AddTransition(FName EventName, FTransitionData Data) { this->Transitions.Add(EventName, Data); }

	void Initialize(UStateMachine* POwner);
	void Enter();
	void EnterWithData(UObject* Data);
	void Exit();
	void ExitWithData(UObject* Data);

	UFUNCTION(BlueprintCallable, Category="StateMachine")
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

	#if WITH_EDITOR
		TArray<FString> GetPropertiesOptions(const FSMPropertySearch& SearchParam) const override;
	#endif
	virtual FSMPropertyReference GetStateMachineProperty(FString& Address) const override;
};

UCLASS(BlueprintType, Abstract, Category = "StateMachine")
class CRABTOOLSUE5_API UTransitionCondition : public UObject
{
	GENERATED_BODY()

	UPROPERTY(Transient, DuplicateTransient)
	TObjectPtr<UStateMachine> OwnerMachine;

public:

	void Initialize(UStateMachine* NewOwner);
	virtual bool Check() const { return false; }

	UFUNCTION(BlueprintCallable, Category = "StateMachine|Transition")
	UObject* GetOwner() const;

	UFUNCTION(BlueprintCallable, Category = "StateMachine|Transition")
	AActor* GetActorOwner() const;

	UFUNCTION(BlueprintNativeEvent, Category="StateMachine|Transition")
	void OnTransitionTaken();
	void OnTransitionTaken_Implementation() {}

	UFUNCTION(BlueprintCallable, Category="StateMachine|Transition")
	UStateMachine* GetMachine() const { return this->OwnerMachine; }

protected:

	UFUNCTION(BlueprintNativeEvent, Category = "StateMachine|Transition")
	void Initialize_Inner();
	virtual void Initialize_Inner_Implementation() {}
};

UCLASS(BlueprintType, Abstract, Category = "StateMachine")
class CRABTOOLSUE5_API UTransitionDataCondition : public UObject
{
	GENERATED_BODY()

	UPROPERTY(Transient, DuplicateTransient)
	TObjectPtr<UStateMachine> OwnerMachine;

public:

	void Initialize(UStateMachine* Owner);
	virtual bool Check(UObject* Data) const { return false; }

	UFUNCTION(BlueprintCallable, Category = "StateMachine|Transition")
	UObject* GetOwner() const;

	UFUNCTION(BlueprintCallable, Category = "StateMachine|Transition")
	AActor* GetActorOwner() const;

	UFUNCTION(BlueprintNativeEvent, Category = "StateMachine|Transition")
	void OnTransitionTaken(UObject* Data);
	void OnTransitionTaken_Implementation(UObject* Data) {}

	UFUNCTION(BlueprintCallable, Category = "StateMachine|Transition")
	FORCEINLINE UStateMachine* GetMachine() const { return this->OwnerMachine; }

protected:

	UFUNCTION(BlueprintNativeEvent, Category="StateMachine|Transition")
	void Initialize_Inner();
	virtual void Initialize_Inner_Implementation() {}
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

	bool bActive = false;

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
	FORCEINLINE bool Active() const { return this->bActive; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateMachine")
	UObject* GetOwner() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateMachine")
	AActor* GetActorOwner() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateMachine")
	UStateMachine* GetMachine() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateMachine")
	UStateMachine* GetRootMachine() const;

	UFUNCTION(BlueprintCallable, Category = "RPG", meta = (ExpandEnumAsExecs = "Result", DeterminesOutputType = "SClass"))
	UStateMachine* GetMachineAs(TSubclassOf<UStateMachine> SClass, ESearchResult& Result) const;

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "StateMachine",
		meta = (HideSelfPin=true))
	void EmitEvent(FName EName);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateMachine")
	UState* GetState() const;

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "StateMachine",
		meta = (HideSelfPin=true))
	void EmitEventWithData(FName EName, UObject* Data);

	UFUNCTION(BlueprintCallable, Category = "StateMachine", meta = (HideSelfPin=true))
	void EmitEventSlot(const FEventSlot& ESlot);

	UFUNCTION(BlueprintCallable, Category = "StateMachine", meta = (HideSelfPin=true))
	void EmitEventSlotWithData(const FEventSlot& ESlot, UObject* Data);

	UFUNCTION(BlueprintNativeEvent, Category = "StateMachine")
	bool RequiresTick() const;
	virtual bool RequiresTick_Implementation() const;

	#if WITH_EDITOR
		virtual void GetNotifies(TSet<FName>& Events) const;
		virtual void GetEmittedEvents(TSet<FName>& Events) const;
		virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
		virtual void PreEditChange(FProperty* PropertyAboutToChange) override;
		TArray<FString> GetPropertyOptions(const FSMPropertySearch& Params) const;
	#endif

	void Event(FName EName);
	void EventWithData(FName EName, UObject* Data);
	void Enter();
	void EnterWithData(UObject* Data);
	void Tick(float DeltaTime);
	void Exit();
	void ExitWithData(UObject* Data);
	void PostTransition();
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
	void Event_Inner(FName EName);	
	virtual void Event_Inner_Implementation(FName EName);

	UFUNCTION(BlueprintNativeEvent, Category = "StateMachine", meta = (DisplayName = "EventWithData"))
	void EventWithData_Inner(FName EName, UObject* Data);
	virtual void EventWithData_Inner_Implementation(FName EName, UObject* Data);

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

	UFUNCTION(BlueprintNativeEvent, Category = "StateMachine", meta = (DisplayName = "PostTransition"))
	void PostTransition_Inner();
	virtual void PostTransition_Inner_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StateMachine", meta = (DisplayName = "ExitWithData"))
	void ExitWithData_Inner(UObject* Data);	
	virtual void ExitWithData_Inner_Implementation(UObject* Data);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StateMachine")
	void RenameEvent(FName Old, FName New);
	virtual void RenameEvent_Implementation(FName Old, FName New);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StateMachine")
	void DeleteEvent(FName Event);
	virtual void DeleteEvent_Implementation(FName Event);

	UFUNCTION(BlueprintNativeEvent, Category = "StateMachine", meta = (DisplayName = "OnSetActive"))
	void SetActive_Inner(bool bNewActive);
	virtual void SetActive_Inner_Implementation(bool bNewActive) {}

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
	void EventWithDataNotifySignatureFunction(FName Name) {}

	void InitNotifies();
};


/**
 * State Machine class. This is the base class for any State Machine, and manages all
 * appropriate state machine behaviour.
 */
UCLASS(Blueprintable, EditInlineNew, Category = "StateMachine")
class CRABTOOLSUE5_API UStateMachine 
: public UObject, public IEventListenerInterface, public IStateMachineLike
{
	GENERATED_BODY()

private:
	#if STATEMACHINE_DEBUG_DATA
		/* Stack data used for debugging state machine transition sequences. */
		FStateMachineDebugDataStack DebugData;

		/* Whether or not this SM was initialized. Used for debugging only. */
		bool bWasInitialized = false;
	#endif

	/* Struct used for identifying unique states. */
	struct Transition
	{
	private:
		int ID = 0;

	public:
		int EnterTransition()
		{
			this->ID += 1;
			return ID;
		}

		bool Valid(int OID) const
		{
			return OID == this->ID;
		}

		int CurrentID() const
		{
			return this->ID;
		}
	} TransitionIdentifier;

	/* Whether or not a transition is happening. */
	bool bIsTransitioning = false;

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

	UPROPERTY(Transient, meta=(IgnorePropertySearch))
	TObjectPtr<UObject> Owner;

	/* Map of name to submachines available to this state machine. */
	UPROPERTY(DuplicateTransient, meta=(IgnorePropertySearch))
	TMap<FName, TObjectPtr<UStateMachine>> SubMachines;

	/* Reference to a parent which uses this state machine as a sub machine. */
	TObjectPtr<UStateMachine> ParentMachine;
	/* The key/name of this submachine in the parent. */
	FName ParentKey;
	/* Sequence of States that this machine has passed through*/
	TDoubleLinkedList<FName> StateStack;

public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTickRequirementUpdated, bool, NeedsTick);
	FTickRequirementUpdated OnTickRequirementUpdated;

	//UPROPERTY(VisibleAnywhere, meta=(IgnorePropertySearch))
	//FName StartState;

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

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	void SendEvent(FName EName);
	virtual void Event_Implementation(FName EName) override final { this->SendEvent(EName); }

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	void SendEventWithData(FName EName, UObject* Data);
	void EventWithData_Implementation(FName EName, UObject* Data) override final { this->SendEventWithData(EName, Data); }

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

	UFUNCTION(BlueprintCallable, Category="StateMachine")
	bool IsActiveState(const UState* State) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateMachine")
	FName GetCurrentStateName() const;

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	UStateMachine* GetParentMachine() const { return this->ParentMachine; }
	void SetParentMachine(UStateMachine* Machine) { this->ParentMachine = Machine; }

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	TArray<FString> StateOptions();

	UFUNCTION(BlueprintNativeEvent, Category="StateMachine")
	void StateChanged(const FStateChangedEventData& Data);
	void StateChanged_Implementation(const FStateChangedEventData& Data) {}

	UFUNCTION(BlueprintNativeEvent, Category = "StateMachine")
	void PostTransition();
	void PostTransition_Implementation() {}

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

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	int GetStateID() { return this->TransitionIdentifier.CurrentID(); }

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	bool IsInState(int ID) { return this->TransitionIdentifier.Valid(ID); }

	TSet<FName> GetEvents() const;
	TMap<FName, TObjectPtr<UStateNode>> GetSharedNodes() { return this->SharedNodes; }

	void SetParentData(UStateMachine* Parent, FName NewParentKey);
	UStateMachineBlueprintGeneratedClass* GetGeneratedClass() const;

	#if WITH_EDITOR
		virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
		virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent) override;
		virtual void PreEditChange(FProperty* PropertyAboutToChange) override;
		virtual void PostLinkerChange() override;
	#endif

	// IStateMachineLike interface
	virtual TArray<FString> GetStateOptions(const UObject* Asker) const override;
	#if WITH_EDITOR
		virtual TArray<FString> GetPropertiesOptions(const FSMPropertySearch& SearchParam) const override;
	#endif //WITH_EDITOR
	virtual FSMPropertyReference GetStateMachineProperty(FString& Address) const override;
	virtual IStateMachineLike* GetSubMachine(FString& Address) const override;

	UFUNCTION(BlueprintCallable, Category="StateMachine")
	bool IsTransitioning() const { return this->bIsTransitioning; }

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

	#if STATEMACHINE_DEBUG_DATA
		const FStateMachineDebugDataStack& GetDebugData() const { return this->DebugData; }
	#endif

	virtual UWorld* GetWorld() const override;

	void AddEventEmitter(UEventEmitter* Emitter) { this->EventEmitters.Add(Emitter); }

protected:

	FName GetStartState() const;

	UFUNCTION(BlueprintNativeEvent, Category = "StateMachine")
	void Initialize_Inner();
	virtual void Initialize_Inner_Implementation();

private:

	void InitFromArchetype();
	void PushStateToStack(FName EName);
	void UpdateState(FName Name);
	void UpdateStateWithData(FName Name, UObject* Data, bool UsePiped=true);
	void InitSubMachines();
};


UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced, CollapseCategories, Category = "StateMachine",
	Within=StateNode)
class CRABTOOLSUE5_API UStateMachineProperty : public UObject
{
	GENERATED_BODY()

private:

	bool bDidInit = false;
	FSMPropertyReference Ref;

public:

	/* The name of the property to get FMovetoData from. */
	UPROPERTY(EditAnywhere, Category = "StateMachine", meta = (GetOptions = "DoPropertySearch"))
	FName Name;	
	FSMPropertySearch Params;

	UStateNode* GetNode() const;

	const FSMPropertyReference& GetProperty();

	UFUNCTION()
	TArray<FString> DoPropertySearch() const;

	void Verify(FNodeVerificationContext& Context);
};