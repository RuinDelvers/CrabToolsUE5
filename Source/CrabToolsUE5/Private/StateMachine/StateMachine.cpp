#include "StateMachine/StateMachine.h"

#include "StateMachine/DataStructures.h"
#include "StateMachine/StateMachineBlueprintGeneratedClass.h"
#include "StateMachine/ArrayNode.h"
#include "StateMachine/Logging.h"
#include "StateMachine/IStateMachineLike.h"
#include "StateMachine/DataStructures.h"
#include "StateMachine/Events.h"
#include "StateMachine/Transitions/BaseTransitions.h"
#include "Utils/UtilsLibrary.h"

#if WITH_EDITOR
	#include "Logging/MessageLog.h"
#endif

#define LOCTEXT_NAMESPACE "StateMachine"

#pragma region StateMachine Code

UStateMachine::UStateMachine(const FObjectInitializer& ObjectInitializer)
{

}

void UStateMachine::InitSubMachines()
{
	if (auto BPGC = Cast<UStateMachineBlueprintGeneratedClass>(this->GetClass()))
	{
		for (auto& Key : BPGC->GetSubMachineOptions())
		{
			auto Machine = BPGC->ConstructSubMachine(this, Key);
			this->SubMachines.Add(Key, Machine);
		}
	}

	for (auto& SubMachine : this->SubMachines)
	{
		if (SubMachine.Value)
		{
			SubMachine.Value->SetParentData(this, SubMachine.Key);
			SubMachine.Value->Initialize(this->Owner);
		}
	}
}

void UStateMachine::Initialize(UObject* POwner)
{
	if (IsValid(POwner))
	{		
		this->Owner = POwner;	
		this->InitFromArchetype();	
		this->Initialize_Inner();

		for (auto& Emitter : this->EventEmitters)
		{
			Emitter->Initialize(this);
		}

		this->InitSubMachines();
	
		// Shared nodes always exist, and should be initializ e from the beginning.
		for (auto& Node : this->SharedNodes)
		{
			Node.Value->Initialize(this);
		}		

		#if STATEMACHINE_DEBUG_DATA
			this->bWasInitialized = true;
		#endif


		this->Reset();
	}
	else
	{
		FMessageLog Log("PIE");
		Log.Error(FText::FormatNamed(
			LOCTEXT("InvalidOwnerError", "Invalid Owner passed to Initialize in {Owner}"),
			TEXT("Owner"), FText::FromString(this->GetClass()->GetName())));
	}	
}

FName UStateMachine::GetStartState() const
{
	if (this->CachedStartState.IsNone())
	{
		if (this->ParentKey.IsNone())
		{
			if (auto BPGC = Cast<UStateMachineBlueprintGeneratedClass>(this->GetClass()))
			{
				this->CachedStartState = BPGC->GetStartState();
			}
		}
		else
		{
			if (auto BPGC = Cast<UStateMachineBlueprintGeneratedClass>(this->ParentMachine->GetClass()))
			{
				this->CachedStartState = BPGC->GetStartState(this->ParentKey);
			}
		}
	}

	return this->CachedStartState;
}

void UStateMachine::Initialize_Inner_Implementation() {}

UStateMachine* UStateNode::GetMachineAs(TSubclassOf<UStateMachine> SClass, bool& bFound) const
{
	auto Class = SClass.Get();
	auto Machine = this->GetMachine();

	if (Class && Machine) {
		if (Machine->IsA(Class)) {
			bFound = true;
			return Machine;
		}
	}

	bFound = false;
	return nullptr;
}

UStateMachine* UStateNode::GetRootMachineAs(TSubclassOf<UStateMachine> SClass, bool& bFound) const
{
	auto Class = SClass.Get();
	auto Machine = this->GetRootMachine();

	if (Class && Machine)
	{
		if (Machine->IsA(Class))
		{
			bFound = true;
			return Machine;
		}
	}

	bFound = false;
	return nullptr;
}

AActor* UStateMachine::GetActorOwner() const {
	return CastChecked<AActor>(this->Owner);
}

UObject* UStateMachine::GetOwner() const
{
	return this->Owner;
}

UStateMachine* UStateMachine::GetSubMachine(const FSubMachineSlot& Slot) const
{
	FString Address = Slot.MachineName.ToString();
	return Cast<UStateMachine>(this->GetSubMachine(Address));
}

UState* UStateMachine::MakeState(FName StateName)
{
	auto State = NewObject<UState>(this);
	this->Graph.Add(StateName, State);
	return State;
}

void UStateMachine::UpdateState()
{
	#if STATEMACHINE_DEBUG_DATA
		if (!this->bWasInitialized)
		{
			this->NotInitializedError();
			return;
		}
	#endif
		
	// If the destination is none or indifferent, then do not update anything.
	if (this->Queue.Destination.IsNone() || this->Queue.Destination == this->CurrentStateName)
	{
		this->Queue.Clear();
		return;
	}

	// Cache the queue to detect preemptions on the transition.
	FTransitionQueue Cached = this->Queue;
	auto CurrentState = this->GetCurrentState();
	this->TransPhase = TransitionPhase::Exiting;
	this->SetupStateChangedData();
	this->UpdateTransitionPipedData(CurrentState, Cached);

	// Exit Phase: Do an atomic exit call on the node.
	if (CurrentState)
	{
		if (Cached.bHasData)
		{
			CurrentState->ExitWithData(Cached.Data);
		}
		else
		{
			CurrentState->Exit();
		}
	}

	// Update the cached data, and see if it's been changed to a new destination.
	this->GetPreemptTransition(Cached);

	this->CurrentStateName = Cached.Destination;
	this->PushStateToStack(Cached.Destination);
	CurrentState = this->GetCurrentState();
	StateChangedData.ToState = CurrentState;
	StateChangedData.To = this->CurrentStateName;
	this->TransPhase = TransitionPhase::Entering;

	if (CurrentState)
	{
		if (Cached.bHasData)
		{
			CurrentState->EnterWithData(Cached.Data);
		}
		else
		{
			CurrentState->Enter();
		}

		this->UpdateTickRequirements(CurrentState->RequiresTick());
	}

	if (Cached.Transition) { Cached.Transition->OnTransitionTaken(); }
	this->ActiveTime = this->GetWorld()->GetTimeSeconds();
	this->OnStateChanged.Broadcast(StateChangedData);
	this->OnTransitionFinished.Broadcast(this);
	this->StateChangedData = FStateChangedEventData();
	this->TransPhase = TransitionPhase::None;

	
	if (this->DidPreempt(Cached))
	{
		this->UpdateState();
	}
	else
	{
		this->Queue.Clear();

		FEventQueueData QueuedEvent;

		while (!this->EventQueue.IsEmpty())
		{
			QueuedEvent = this->EventQueue.Pop();

			if (QueuedEvent.bHasData)
			{
				this->SendEventWithData(QueuedEvent.EventName, QueuedEvent.Data);
			}
			else
			{
				this->SendEvent(QueuedEvent.EventName);
			}
		}
	}
}

void UStateMachine::UpdateTransitionPipedData(UState* CurrentState, FTransitionQueue& Cached)
{
	if (CurrentState->HasPipedData())
	{
		auto PipedData = CurrentState->GetPipedData();

		if (Queue.bHasData)
		{
			auto ArrayData = NewObject<UArrayNodeData>(this);
			ArrayData->AddData(Cached.Data);
			ArrayData->AddData(PipedData);
			PipedData = PipedData;
		}

		Cached.Data = PipedData;
		Cached.bHasData = true;
	}
}

void UStateMachine::SetupStateChangedData()
{
	if (this->TransPhase == TransitionPhase::None)
	{
		this->EventQueue.Empty();
		this->TransPhase = TransitionPhase::Exiting;
		StateChangedData.StateMachine = this;
		StateChangedData.From = this->CurrentStateName;
		StateChangedData.To = this->Queue.Destination;
	}
}

bool UStateMachine::DidPreempt(const FTransitionQueue& Cached) const
{
	return !Queue.Destination.IsNone() && (Cached.TransitionID != this->Queue.TransitionID);
}

void UStateMachine::GetPreemptTransition(FTransitionQueue& Cached) const
{
	if (this->DidPreempt(Cached))
	{
		Cached = this->Queue;
	}
}

void UStateMachine::Tick(float DeltaTime)
{
	if (this->bIsActive)
	{
		auto State = this->GetCurrentState();

		if (State)
		{
			State->Tick(DeltaTime);
		}

		this->UpdateState();
	}
}


void UStateMachine::Reset()
{
	this->Queue.Queue(this->GetStartState());
	this->UpdateState();
}

UStateMachine* UStateMachine::GetRootMachine()
{
	if (this->ParentMachine)
	{
		return this->ParentMachine->GetRootMachine();
	}
	else
	{
		return this;
	}
}

void UStateMachine::SendEvent(FName InEvent)
{
	#if STATEMACHINE_DEBUG_DATA
		if (!this->bWasInitialized)
		{
			this->NotInitializedError();
			return;
		}
	#endif

	if (!this->bIsActive) { return; }
	if (InEvent.IsNone()) { return; }

	auto CurrentState = this->GetCurrentState();

	if (CurrentState)
	{
		#if WITH_EDITORONLY_DATA
			CurrentState->OnEventReceived.Broadcast(InEvent);
		#endif

		FDestinationResult Result;

		// First we check if there are any declarative events to handle for this state.
		if (CurrentState->GetDestination(InEvent, Result))
		{
			if (!Result.Destination.IsNone())
			{
				this->Queue.Queue(Result.Destination, Result.Condition, InEvent);
			}
		}
		else
		{
			if (this->TransPhase == TransitionPhase::Entering)
			{
				this->EventQueue.Add(InEvent);
			}
			else
			{
				CurrentState->Event(InEvent);
			}
		}

		this->UpdateState();
	}
}

void UStateMachine::SendEventWithData(FName InEvent, UObject* Data)
{
	#if STATEMACHINE_DEBUG_DATA
		if (!this->bWasInitialized)
		{
			this->NotInitializedError();
			return;
		}
	#endif

	if (!this->bIsActive) { return; }
	if (InEvent.IsNone()) { return; }

	auto CurrentState = this->GetCurrentState();

	if (CurrentState)
	{
		#if WITH_EDITORONLY_DATA
			CurrentState->OnEventWithDataReceived.Broadcast(InEvent, Data);
		#endif

		FDestinationResult Result;

		// First we check if there are any declarative events to handle for this state.
		if (CurrentState->GetDataDestination(InEvent, Data, Result))
		{
			if (!Result.Destination.IsNone())
			{
				this->Queue.Queue(Result.Destination, Data, InEvent, Result.Condition);
			}
		}
		else
		{
			if (this->TransPhase == TransitionPhase::Entering)
			{
				this->EventQueue.Add(InEvent);
			}
			else
			{
				CurrentState->EventWithData(InEvent, Data);
			}
		}
		this->UpdateState();
	}
}

void UStateMachine::UpdateTickRequirements(bool NeedsTick)
{
	this->OnTickRequirementUpdated.Broadcast(NeedsTick);

	if (this->ParentMachine)
	{
		this->ParentMachine->UpdateTickRequirements(NeedsTick);
	}
}

void UStateMachine::SetActive(bool bNewActive)
{
	if (this->TransPhase == TransitionPhase::None)
	{
		if (!bNewActive && this->ActiveDefaultEvents.Contains(EDefaultStateMachineEvents::ON_MACHINE_DEACTIVATE))
		{
			this->SendEvent(Events::Default::MACHINE_DEACTIVATED);
		}

		this->bIsActive = bNewActive;

		if (auto State = this->GetCurrentState())
		{
			if (State->GetNode())
			{
				State->GetNode()->SetActive(bNewActive);
			}
		}

		if (bNewActive && this->ActiveDefaultEvents.Contains(EDefaultStateMachineEvents::ON_MACHINE_ACTIVATE))
		{
			this->SendEvent(Events::Default::MACHINE_ACTIVATED);
		}
	}
}

bool UStateMachine::IsActiveState(const UState* State) const
{
	return this->GetCurrentStateData() == State;
}

#if WITH_EDITOR
void UStateMachine::PreEditChange(FProperty* PropertyAboutToChange)
{
	Super::PreEditChange(PropertyAboutToChange);
}

void UStateMachine::PostEditChangeChainProperty(struct FPropertyChangedChainEvent& e)
{
	Super::PostEditChangeChainProperty(e);
}

void UStateMachine::PostEditChangeProperty(struct FPropertyChangedEvent& e)
{
    Super::PostEditChangeProperty(e);
}

void UStateMachine::PostLinkerChange()
{
	Super::PostLinkerChange();
}

void UStateMachine::AppendDefaultEvents(const TSet<EDefaultStateMachineEvents>& Events)
{
	this->ActiveDefaultEvents.Append(Events);
}


#endif // WITH_EDITOR

#if STATEMACHINE_DEBUG_DATA
void UStateMachine::NotInitializedError()
{
	FMessageLog Log("PIE");
	Log.Error(
		FText::FormatNamed(
			LOCTEXT("NotInitializedError", "Failed to Initialize StateMachine before usage in {Owner}"),
			TEXT("Owner"), FText::FromString(this->GetName())));
}
#endif // STATEMACHINE_DEBUG_DATA


UState* UStateMachine::GetStateData(FName Name)
{
	if (auto Data = this->Graph.Find(Name))
	{
		return Data->Get();
	}
	else
	{
		return nullptr;
	}
}


void UStateMachine::InitFromArchetype()
{	
	if (auto BPGC = this->GetGeneratedClass())
	{
		BPGC->AppendEventEmitters(this);
	}
}

TArray<FString> UStateMachine::StateOptions()
{
	TArray<FString> Names;

	for (const auto& Nodes : this->Graph) {
		Names.Add(Nodes.Key.ToString());
	}

	if (auto BPGC = Cast<UStateMachineBlueprintGeneratedClass>(this->GetClass()))
	{
		Names.Append(BPGC->Archetype.GetArchetype()->StateOptions());
	}

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}

TArray<FString> UStateMachine::ConditionOptions() const {
	TArray<FString> Names;

	auto base = this->FindFunction("TrueCondition");

	for (TFieldIterator<UFunction> FIT(this->GetClass(), EFieldIteratorFlags::IncludeSuper); FIT; ++FIT) {
		UFunction* f = *FIT;
		
		if (f->IsSignatureCompatibleWith(base)) {
			Names.Add(f->GetName());
		}		
	}

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}

TArray<FString> UStateMachine::ConditionDataOptions() const {
	TArray<FString> Names;
	auto base = this->FindFunction("TrueDataCondition");

	for (TFieldIterator<UFunction> FIT(this->GetClass(), EFieldIteratorFlags::IncludeSuper); FIT; ++FIT) {
		UFunction* f = *FIT;		
		
		if (f->IsSignatureCompatibleWith(base)) {
			Names.Add(f->GetName());
		}
	}

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}

bool UStateMachine::TrueCondition() { 
	return true; 
}

bool UStateMachine::FalseCondition() {
	return false;
}

bool UStateMachine::TrueDataCondition(UObject* Data)
{
	return true;
}

bool UStateMachine::FalseDataCondition(UObject* Data)
{
	return false;
}

bool UStateMachine::ValidDataCondition(UObject* Data)
{
	return IsValid(Data);
}

TSet<FName> UStateMachine::GetEvents() const {
	TSet<FName> List;

	if (auto BPGC = this->GetGeneratedClass())
	{
		List.Append(BPGC->GetTotalEventSet());
	}

	for (const auto& States : this->Graph)
	{
		States.Value->GetTransitionEvents(List);
	}

	return List;
}

void UStateMachine::AddStateData(FName StateName, UState* Data)
{
	if (Data->GetOuter() != this)
	{
		Data = DuplicateObject(Data, this);
	}

	if (!this->Graph.Contains(StateName))
	{
		this->Graph.Add(StateName, Data);
	}
}

UStateMachine* UStateMachine::FindMachineByClass(TSubclassOf<UStateMachine> MachineClass)
{
	if (this->IsA(MachineClass))
	{
		return this;
	}
	else
	{
		for (const auto& SubMachine : this->SubMachines)
		{
			if (auto Found = SubMachine.Value->FindMachineByClass(MachineClass))
			{
				return Found;
			}
		}
	}

	return nullptr;
}


UStateMachine* UStateMachine::FindMachineByInterface(TSubclassOf<UInterface> MachineClass)
{
	if (this->GetClass()->ImplementsInterface(MachineClass))
	{
		return this;
	}
	else
	{
		for (const auto& SubMachine : this->SubMachines)
		{
			if (auto Found = SubMachine.Value->FindMachineByInterface(MachineClass))
			{
				return Found;
			}
		}
	}

	return nullptr;
}

UState* UStateMachine::MakeStateWithNode(FName StateName, UStateNode* Node)
{
	UState* Data = NewObject<UState>(this);
	Data->SetNode(Cast<UStateNode>(DuplicateObject(Node, this)));
	this->Graph.Add(StateName, Data);

	return Data;
}

UState* UStateMachine::GetCurrentStateData() const
{
	if (auto Data = this->Graph.Find(this->CurrentStateName))
	{
		return Data->Get();
	}
	else
	{
		return nullptr;
	}
}

FName UStateMachine::GetCurrentStateName() const
{
	return this->CurrentStateName;
}

UStateNode* UStateMachine::GetCurrentStateAs(TSubclassOf<UStateNode> Class, ESearchResult& Branches)
{
	auto Node = this->GetCurrentState();

	if (Class.Get() && Node)
	{
		if (Node->HasNode() && Node->GetNode()->IsA(Class.Get()->StaticClass()))
		{
			Branches = ESearchResult::Found;

			return Node->GetNode();
		}
	}

	Branches = ESearchResult::NotFound;
	return nullptr;
}

UState* UStateMachine::GetCurrentState()
{
	auto FoundData = this->Graph.Find(this->CurrentStateName);

	if (FoundData)
	{
		return FoundData->Get();
	}
	else
	{
		UState* BuiltState = nullptr;

		// First get the data for this state from the Generated class if it exists.
		if (auto BPGC = Cast<UStateMachineBlueprintGeneratedClass>(this->GetClass()))
		{			
			BuiltState = BPGC->GetStateData(this, NAME_None, this->CurrentStateName);
		}
		
		// Then check to see if there's a parent machine to get extra state data from.
		if (IsValid(this->ParentMachine))
		{
			if (auto ParentBPGC = Cast<UStateMachineBlueprintGeneratedClass>(this->ParentMachine->GetClass()))
			{
				
				auto SubState = ParentBPGC->GetStateData(this, this->ParentKey, this->CurrentStateName);

				if (IsValid(SubState))
				{
					if (IsValid(BuiltState))
					{
						BuiltState->Append(SubState);
					}
					else
					{
						BuiltState = SubState;
					}
				}
			}
		}

		if (!IsValid(BuiltState))
		{
			BuiltState = NewObject<UState>();
		}

		BuiltState->Initialize(this);

		this->Graph.Add(this->CurrentStateName, BuiltState);
		return BuiltState;
	}
	

	return nullptr;
}



IStateMachineLike* UStateMachine::GetSubMachine(FString& Address) const
{
	FString Base;
	FString Target;

	if (Address.Split(TEXT("/"), &Base, &Target))
	{
		FName BaseName(Base);

		if (auto SubM = this->SubMachines.Find(BaseName))
		{
			return SubM->Get();
		}
		else if (Base == "..")
		{
			if (this->ParentMachine)
			{
				return this->ParentMachine->GetSubMachine(Target);
			}
			else
			{
				return nullptr;
			}
		}
		else
		{
			return nullptr;
		}
	}
	else
	{
		FName Key(Address);

		if (auto Found = this->SubMachines.Find(Key))
		{
			return Found->Get();
		}
		else if (!Key.IsNone())
		{
			return nullptr;
		}
		else
		{
			return nullptr;
		}
	}
}

void UStateMachine::BindConditionAt(FString& Address, FTransitionDelegate& Condition)
{
	FString RootPath;
	FString ConditionPath;

	if (Address.Split("/", &RootPath, &ConditionPath))
	{
		if (RootPath == "..")
		{
			if (this->ParentMachine)
			{
				this->ParentMachine->BindConditionAt(ConditionPath, Condition);
			}
			else
			{
				UE_LOG(
					LogStateMachine, 
					Error, 
					TEXT("Found Condition for parent machine with null parent: %s"),
					*Address);
			}
		}
		else
		{
			if (auto SubPtr = this->SubMachines.Find(FName(RootPath)))
			{
				if (auto SubMachine = SubPtr->Get())
				{
					SubMachine->BindConditionAt(ConditionPath, Condition);
				}
			}
		}
	}
	else
	{
		Condition.BindUFunction(this, FName(Address));
	}
}

void UStateMachine::BindDataConditionAt(FString& Address, FTransitionDataDelegate& Condition)
{
	FString RootPath;
	FString ConditionPath;

	if (Address.Split("/", &RootPath, &ConditionPath))
	{
		if (RootPath == "..")
		{
			if (this->ParentMachine)
			{
				this->ParentMachine->BindDataConditionAt(ConditionPath, Condition);
			}
			else
			{
				UE_LOG(
					LogStateMachine,
					Error,
					TEXT("Found Condition for parent machine with null parent: %s"),
					*Address);
			}
		}
		else
		{
			if (auto SubPtr = this->SubMachines.Find(FName(RootPath)))
			{
				if (auto SubMachine = SubPtr->Get())
				{
					SubMachine->BindDataConditionAt(ConditionPath, Condition);
				}
			}
		}
	}
	else
	{
		Condition.BindUFunction(this, FName(Address));
	}
}

void UStateMachine::PushStateToStack(FName InEvent)
{
	if (this->StateStack.Num() >= this->MaxPrevStateStackSize)
	{
		this->StateStack.RemoveNode(this->StateStack.GetHead());
	}

	this->StateStack.AddTail(InEvent);
}

FName UStateMachine::GetPreviousStateName() const
{
	if (this->StateStack.Num() <= 1)
	{
		return NAME_None;
	}
	else
	{
		return this->StateStack.GetTail()->GetPrevNode()->GetValue();
	}
}

UState* UStateMachine::GetPreviousState() const
{
	if (this->StateStack.Num() <= 1)
	{
		return nullptr;
	}
	else
	{
		FName PrevName = this->StateStack.GetTail()->GetPrevNode()->GetValue();
		return this->Graph[PrevName];
	}
}

UState* UStateMachine::DuplicateStateObject(FName StateName, UObject* NewOuter) const
{
	if (auto Data = this->Graph.Find(StateName))
	{
		return DuplicateObject(Data->Get(), NewOuter);
	}
	else
	{
		return nullptr;
	}
}

void UStateMachine::SetParentData(UStateMachine* Parent, FName NewParentKey)
{
	this->ParentMachine = Parent;
	this->ParentKey = NewParentKey;
}

UStateMachineBlueprintGeneratedClass* UStateMachine::GetGeneratedClass() const
{
	return Cast<UStateMachineBlueprintGeneratedClass>(this->GetClass());
}

TArray<FString> UStateMachine::GetStateOptions(const UObject* Asker) const
{
	TArray<FString> Names;

	if (auto BPGC = this->GetGeneratedClass())
	{
		if (Asker)
		{
			if (Asker->IsIn(this))
			{
				Names.Append(BPGC->GetStateOptions(EStateMachineAccessibility::PROTECTED));
			}
			else
			{
				Names.Append(BPGC->GetStateOptions(EStateMachineAccessibility::PUBLIC));
			}
		}
		else
		{
			Names.Append(BPGC->GetStateOptions(EStateMachineAccessibility::PUBLIC));
		}
	}

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}

UWorld* UStateMachine::GetWorld() const
{
	//Return null if the called from the CDO, or if the outer is being destroyed
	if (!HasAnyFlags(RF_ClassDefaultObject) && !GetOuter()->HasAnyFlags(RF_BeginDestroyed) && !GetOuter()->IsUnreachable())
	{
		//Try to get the world from the owning actor if we have one
		if (this->Owner)
		{
			return this->GetActorOwner()->GetWorld();
		}
	}

	//Else return null - the latent action will fail to initialize
	return nullptr;
}

void FTransitionQueue::Queue(FName NewDestination, FName Event, UAbstractCondition* TakenTransition)
{
	this->Destination = NewDestination;
	this->bHasData = false;
	this->Data = nullptr;
	this->Transition = TakenTransition;
	this->TransitionID += 1;
	this->CurrentEvent = Event;
}

void FTransitionQueue::Queue(FName NewDestination, UObject* NewData, FName Event, UAbstractCondition* TakenTransition)
{
	this->Destination = NewDestination;
	this->bHasData = true;
	this->Data = NewData;
	this->Transition = TakenTransition;
	this->TransitionID += 1;
	this->CurrentEvent = Event;
}

void FTransitionQueue::Clear()
{
	this->Destination = NAME_None;
	this->bHasData = false;
	this->Data = nullptr;
	this->CurrentEvent = NAME_None;
	this->TransitionID = 0;
}

#pragma endregion

#pragma region NodeCode

UStateNode::UStateNode()
{

}

void UStateNode::Initialize(UStateMachine* POwner)
{
	this->Owner = POwner;
	this->InitNotifies();

	this->Initialize_Inner();
}

void UStateNode::InitNotifies()
{
	auto NotifySign = this->FindFunction(GET_FUNCTION_NAME_CHECKED(UStateNode, EventNotifySignatureFunction));
	auto NotifyWithDataSign = this->FindFunction(GET_FUNCTION_NAME_CHECKED(UStateNode, EventWithDataNotifySignatureFunction));

	for (TFieldIterator<UFunction> FIT(this->GetClass(), EFieldIteratorFlags::IncludeSuper); FIT; ++FIT)
	{
		UFunction* f = *FIT;
		FString FnName = f->GetName();

		if (FnName.StartsWith("EventNotify_"))
		{
			FName EventName(f->GetName().RightChop(12));

			if (f->IsSignatureCompatibleWith(NotifySign))
			{
				FEventNotify Notify;
				FEventNotify_Single Del;
				Del.BindUFunction(this, FName(FnName));

				if (!this->EventNotifies.Contains(EventName))
				{
					this->EventNotifies.Add(EventName, Notify);
				}

				this->EventNotifies.Find(EventName)->AddLambda([this, Del] (FName EventName)
					{
						if (this->Active())
						{
							Del.Execute(EventName);
						}
					});
			}
			else if (f->IsSignatureCompatibleWith(NotifyWithDataSign))
			{
				FEventWithDataNotify Notify;
				FEventWithDataNotify_Single Del;
				Del.BindUFunction(this, FName(FnName));

				if (!this->EventWithDataNotifies.Contains(EventName))
				{
					this->EventWithDataNotifies.Add(EventName, Notify);
				}

				this->EventWithDataNotifies.Find(EventName)->AddLambda([this, Del](FName EventName, UObject* Data)
					{
						if (this->Active())
						{
							Del.Execute(EventName, Data);
						}
					});
			}
		}
	}
}

void UStateNode::Initialize_Inner_Implementation() {}


UStateMachine* UStateNode::GetMachine() const
{
	return this->Owner;
}

UState* UStateNode::GetState() const
{
	return UtilsFunctions::GetOuterAs<UState>(this);
}

UObject* UStateNode::GetOwner() const {
	return this->Owner->GetOwner();
}

AActor* UStateNode::GetActorOwner() const
{
	return this->Owner->GetActorOwner();
}

void UStateNode::Event(FName InEvent)
{
	if (this->Active())
	{
		if (auto Notify = this->EventNotifies.Find(InEvent))
		{
			Notify->Broadcast(InEvent);
		}
		else
		{
			this->Event_Inner(InEvent);
		}
	}
}

UStateMachine* UStateNode::GetRootMachine() const
{
	return this->Owner->GetRootMachine();
}

void UStateNode::Event_Inner_Implementation(FName InEvent)
{
	// Does Nothing by default.
}

void UStateNode::EventWithData(FName InEvent, UObject* Data)
{
	if (this->Active())
	{
		if (auto Notify = this->EventWithDataNotifies.Find(InEvent))
		{
			Notify->Broadcast(InEvent, Data);
		}
		else
		{
			this->EventWithData_Inner(InEvent, Data);
		}
	}
}

void UStateNode::EventWithData_Inner_Implementation(FName InEvent, UObject* Data)
{
	this->Event_Inner(InEvent);
}

bool UStateNode::DoesReferenceMachine(FName MachineName) const
{
	return this->DoesReferenceMachine_Inner(MachineName);
}

void UStateNode::SetOwner(UStateMachine* Parent)
{
	this->Owner = Parent;
}

bool UStateNode::Active() const
{
	switch (this->CurrentState)
	{
		case EStateNodeState::INACTIVE: return false;
		case EStateNodeState::EXITING: return true;
		case EStateNodeState::ENTERING: return true;
		case EStateNodeState::ENTERED: return true;
		case EStateNodeState::ENTERED_INACTIVE: return false;
		default: return false;
	}
}

void UStateNode::SetActive(bool bNewActive)
{
	bool bOldActive = this->Active();

	switch (this->CurrentState)
	{
		case EStateNodeState::ENTERED:
			if (!bNewActive)
			{
				this->CurrentState = EStateNodeState::ENTERED_INACTIVE;
			}
		case EStateNodeState::ENTERED_INACTIVE:
			if (bNewActive)
			{
				this->CurrentState = EStateNodeState::ENTERED;
			}
	}

	bool bComputeActive = this->Active();

	if (bComputeActive != bOldActive)
	{
		this->SetActive_Inner(bComputeActive);
	}
}

void UStateNode::Tick(float DeltaTime)
{
	if (this->Active())
	{
		this->Tick_Inner(DeltaTime);
	}
}

void UStateNode::Exit()
{
	if (
		this->CurrentState == EStateNodeState::ENTERING
		|| this->CurrentState == EStateNodeState::ENTERED
		|| this->CurrentState == EStateNodeState::ENTERED_INACTIVE)
	{
		this->CurrentState = EStateNodeState::EXITING;
		this->Exit_Inner();

		if (this->CurrentState == EStateNodeState::EXITING)
		{
			this->CurrentState = EStateNodeState::INACTIVE;
			this->SetActive_Inner(false);
		}
	}
}

void UStateNode::ExitWithData(UObject* Data)
{
	if (
		this->CurrentState == EStateNodeState::ENTERING
		|| this->CurrentState == EStateNodeState::ENTERED
		|| this->CurrentState == EStateNodeState::ENTERED_INACTIVE)
	{
		this->CurrentState = EStateNodeState::EXITING;
		this->ExitWithData_Inner(Data);

		if (this->CurrentState == EStateNodeState::EXITING)
		{
			this->CurrentState = EStateNodeState::INACTIVE;
			this->SetActive_Inner(false);
		}
	}
}

void UStateNode::ExitWithData_Inner_Implementation(UObject* Data)
{
	this->Exit_Inner();
}

void UStateNode::Enter()
{
	if (this->CurrentState == EStateNodeState::INACTIVE || this->CurrentState == EStateNodeState::EXITING)
	{
		this->CurrentState = EStateNodeState::ENTERING;
		this->Enter_Inner();
		
		if (this->CurrentState == EStateNodeState::ENTERING)
		{
			this->CurrentState = EStateNodeState::ENTERED;
			this->SetActive_Inner(true);
		}
	}
}

void UStateNode::EnterWithData(UObject* Data)
{
	if (this->CurrentState == EStateNodeState::INACTIVE || this->CurrentState == EStateNodeState::EXITING)
	{
		this->CurrentState = EStateNodeState::ENTERING;
		this->EnterWithData_Inner(Data);

		if (this->CurrentState == EStateNodeState::ENTERING)
		{
			this->CurrentState = EStateNodeState::ENTERED;
			this->SetActive_Inner(true);
		}
	}
}

void UStateNode::EnterWithData_Inner_Implementation(UObject* Data)
{
	this->Enter_Inner();
}

bool UStateNode::Verify(FNodeVerificationContext& Context) const
{
	bool bErrorFree = true;

	auto SLike = UtilsFunctions::GetOuterAs<IStateLike>(this);
	auto SMLike = UtilsFunctions::GetOuterAs<IStateMachineLike>(this);

	if (SLike && SMLike)
	{
		for (TFieldIterator<FProperty> FIT(this->GetClass(), EFieldIteratorFlags::IncludeSuper); FIT; ++FIT)
		{
			if (FIT->GetClass() == FStructProperty::StaticClass())
			{
				FStructProperty* f = CastField<FStructProperty>(*FIT);

				if (f->Struct == FSubMachineSlot::StaticStruct())
				{
					FSubMachineSlot Value;
					f->GetValue_InContainer(this, &Value);

					auto Options = SMLike->GetMachineOptions();

					if (!(Value.MachineName.IsNone() || Options.Contains(Value.MachineName)))
					{
						FString Msg = FString::Printf(TEXT("Could not find StateMachine for slot %s: %s"),
							*f->GetName(),
							*this->GetName());
						Context.Error(Msg, this);

						bErrorFree = false;
					}
				}
			}
			else if (FIT->GetClass() == FObjectProperty::StaticClass())
			{

			}
		}
	}
	else
	{
		FString Msg = FString::Printf(TEXT("Could not found outer ISMLike for Node: %s"), *this->GetName());
		Context.Error(Msg, this);

		bErrorFree = false;
	}

	return bErrorFree && this->Verify_Inner(Context);
}

UWorld* UStateNode::GetWorld() const
{
	//Return null if the called from the CDO, or if the outer is being destroyed
	if (!HasAnyFlags(RF_ClassDefaultObject) && !GetOuter()->HasAnyFlags(RF_BeginDestroyed) && !GetOuter()->IsUnreachable())
	{
		//Try to get the world from the owning actor if we have one
		if (this->Owner)
		{
			return this->Owner->GetActorOwner()->GetWorld();
		}
	}

	//Else return null - the latent action will fail to initialize
	return nullptr;
}

void UStateNode::UpdateTickRequirements() const
{
	this->GetMachine()->UpdateTickRequirements(this->RequiresTick());

}

bool UStateNode::RequiresTick_Implementation() const
{
	return false;
}

void UStateNode::EmitEvent(FName InEvent)
{
	if (this->Active())
	{
		this->GetMachine()->SendEvent(InEvent);
	}
}

void UStateNode::EmitEventWithData(FName InEvent, UObject* Data)
{
	if (this->Active())
	{
		this->GetMachine()->SendEventWithData(InEvent, Data);
	}
}

bool UStateNode::Modify(bool bShouldAlwaysMarkDirty)
{
	if (auto StateLike = UtilsFunctions::GetOuterAs<IStateLike>(this))
	{
		StateLike->OnModify();
	}

	return Super::Modify(bShouldAlwaysMarkDirty);
}

#if WITH_EDITOR

void UStateNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (auto Parent = Cast<UStateNode>(this->GetClass()->GetSuperClass()->GetDefaultObject()))
	{
		for (auto& Ev : Parent->EmittedEvents)
		{
			this->AddEmittedEvent(Ev);
		}
	}
}

void UStateNode::PreEditChange(FProperty* PropertyAboutToChange)
{
	Super::PreEditChange(PropertyAboutToChange);
}

TArray<FString> UStateNode::GetMachineOptions() const
{
	const UObject* Outer = this;

	if (auto SMLike = UtilsFunctions::GetOuterAs<IStateMachineLike>(Outer))
	{
		return SMLike->GetMachineOptions();
	}

	return { };
}

void UStateNode::GetNotifies(TSet<FName>& Events) const
{
	auto NotifySign = this->FindFunction(GET_FUNCTION_NAME_CHECKED(UStateNode, EventNotifySignatureFunction));
	auto NotifyWithDataSign = this->FindFunction(GET_FUNCTION_NAME_CHECKED(UStateNode, EventWithDataNotifySignatureFunction));

	for (TFieldIterator<UFunction> FIT(this->GetClass(), EFieldIteratorFlags::IncludeSuper); FIT; ++FIT)
	{
		UFunction* f = *FIT;
		FString FnName = f->GetName();

		if (FnName.StartsWith("EventNotify_"))
		{
			FName EventName(f->GetName().RightChop(12));

			if (f->IsSignatureCompatibleWith(NotifySign))
			{
				Events.Add(EventName);
			}
			else if (f->IsSignatureCompatibleWith(NotifyWithDataSign))
			{
				Events.Add(EventName);
			}
		}
	}
}

TArray<FString> UStateNode::GetEventOptions() const
{
	if (auto StateLike = UtilsFunctions::GetOuterAs<IStateLike>(this))
	{
		return StateLike->GetEventOptions();
	}

	return { FName(NAME_None).ToString() };
}

TArray<FString> UStateNode::GetStateOptions() const
{
	if (auto StateLike = UtilsFunctions::GetOuterAs<IStateMachineLike>(this))
	{
		return StateLike->GetStateOptions(this);
	}

	return { FName(NAME_None).ToString() };
}

TArray<FString> UStateNode::GetIncomingStateOptions() const
{
	if (auto StateLike = UtilsFunctions::GetOuterAs<IStateLike>(this))
	{
		return StateLike->GetEnterStates();
	}

	return { FName(NAME_None).ToString() };
}

TArray<FString> UStateNode::GetOutgoingStateOptions() const
{
	if (auto StateLike = UtilsFunctions::GetOuterAs<IStateLike>(this))
	{
		return StateLike->GetExitStates();
	}

	return { FName(NAME_None).ToString() };
}

#endif // WITH_EDITOR

#if WITH_EDITORONLY_DATA

void UStateNode::GetEmittedEvents(TSet<FName>& Events) const 
{
	Events.Append(this->EmittedEvents);

	for (TFieldIterator<FStructProperty> FIT(this->GetClass(), EFieldIteratorFlags::IncludeSuper); FIT; ++FIT)
	{
		FStructProperty* f = *FIT;

		if (f->Struct == FEventSlot::StaticStruct())
		{
			auto Value = f->ContainerPtrToValuePtr<FEventSlot>(this);
			Events.Add(*Value);
		}
	}
	
}

#endif // WITH_EDITORONLY_DATA

#pragma endregion

void UState::Append(UState* Data)
{
	this->AppendNode(Data->Node);

	for (const auto& Trans : Data->Transitions)
	{
		if (!this->Transitions.Contains(Trans.Key))
		{

			this->Transitions.Add(Trans.Key, Trans.Value);
			this->Transitions[Trans.Key].Copy(this);
		}
		else
		{
			this->Transitions[Trans.Key].Destinations.Append(Trans.Value.Destinations);
			this->Transitions[Trans.Key].Copy(this);
		}
	}
}

void UState::AppendCopy(UState* Data)
{
	this->AppendNodeCopy(Data->Node);

	for (const auto& Trans : Data->Transitions)
	{
		if (!this->Transitions.Contains(Trans.Key))
		{
			this->Transitions.Add(Trans.Key, Trans.Value);
			this->Transitions[Trans.Key].Copy(this);
		}
		else
		{
			this->Transitions[Trans.Key].Destinations.Append(Trans.Value.Destinations);
			this->Transitions[Trans.Key].Copy(this);
		}
	}
}

void UState::AppendNode(UStateNode* ANode)
{
	if (IsValid(ANode))
	{
		if (ANode->GetOuter() != this)
		{
			ANode = DuplicateObject(ANode, this);
		}

		if (auto ArrayNode = Cast<UArrayNode>(this->Node))
		{
			if (IsValid(ANode))
			{
				ArrayNode->AddNode(ANode);
			}
		}
		else
		{
			if (IsValid(this->Node))
			{
				auto NewNode = NewObject<UArrayNode>(this);
				NewNode->AddNode(this->Node);
				NewNode->AddNode(ANode);
				this->Node = NewNode;
			}
			else
			{
				this->Node = ANode;
			}
		}
	}
}

void UState::AppendNodeCopy(UStateNode* ANode)
{
	if (IsValid(ANode))
	{
		auto NewNode = DuplicateObject(ANode, this);
		this->AppendNode(NewNode);
	}
}

void UAbstractCondition::Initialize(UStateMachine* NewOwner, FName ISource, FName IDestination)
{
	this->Source = ISource;
	this->Destination = IDestination;
	UStateNode::Initialize(NewOwner);
}

void UState::AddTransition(FName EventName, FName Destination, FTransitionData Data)
{
	if (!this->Transitions.Contains(EventName))
	{
		this->Transitions.Add(EventName, FTransitionDataSet());
	}

	this->Transitions[EventName].Destinations.Add(Destination, Data);
}

void UState::AddTransition(FName EventName, FTransitionDataSet Data)
{
	if (this->Transitions.Contains(EventName))
	{
		this->Transitions[EventName].Destinations.Append(Data.Destinations);
	}
	else
	{
		this->Transitions.Add(EventName, Data);
	}
}

void UState::AddTransition(const TMap<FName, FTransitionDataSet>& Data)
{
	this->Transitions.Append(Data);
}

void UState::Initialize(UStateMachine* POwner)
{
	this->OwnerMachine = POwner;

	for (auto& Events : this->Transitions)
	{	
		Events.Value.Initialize(POwner, Events.Key);		
	}

	if (IsValid(this->Node))
	{
		this->Node->Initialize(POwner);
	}
}

void UState::Tick(float DeltaTime)
{
	if (IsValid(this->Node))
	{
		this->Node->Tick(DeltaTime);
	}

	for (const auto& Event : this->Transitions)
	{
		for (const auto& Condition : Event.Value.Destinations)
		{
			Condition.Value.Condition->Tick(DeltaTime);
			Condition.Value.DataCondition->Tick(DeltaTime);
		}
	}
}

bool UState::RequiresTick() const
{
	bool NodeTicks =  IsValid(this->Node) ? this->Node->RequiresTick() : false;

	if (!NodeTicks)
	{
		for (const auto& Event : this->Transitions)
		{
			for (const auto& Dest : Event.Value.Destinations)
			{
				NodeTicks = Dest.Value.Condition->RequiresTick()
					|| Dest.Value.DataCondition->RequiresTick();

				if (NodeTicks)
				{
					break;
				}
			}
		}
	}

	return NodeTicks;
}

void UState::EnterConditions()
{
	for (const auto& Transition : this->Transitions)
	{
		for (const auto& Destination : Transition.Value.Destinations)
		{
			Destination.Value.Condition->Enter();
			Destination.Value.DataCondition->Enter();
		}
	}
}

void UState::ExitConditions()
{
	for (const auto& Transition : this->Transitions)
	{
		for (const auto& Destination : Transition.Value.Destinations)
		{
			Destination.Value.Condition->Exit();
			Destination.Value.DataCondition->Exit();
		}
	}
}

void UState::Enter()
{
	this->Enter_Inner();

	if (IsValid(this->Node))
	{
		this->Node->Enter();
	}

	this->EnterConditions();
}

void UState::EnterWithData(UObject* Data)
{
	this->EnterWithData_Inner(Data);

	if (IsValid(this->Node))
	{
		this->Node->EnterWithData(Data);
	}

	this->EnterConditions();
}

void UState::Exit()
{
	this->Exit_Inner();

	if (IsValid(this->Node))
	{
		this->Node->Exit();
	}

	this->ExitConditions();
}

void UState::ExitWithData(UObject* Data)
{
	this->ExitWithData_Inner(Data);

	if (IsValid(this->Node))
	{
		this->Node->ExitWithData(Data);
	}

	this->ExitConditions();
}

void UState::Event(FName InEvent)
{
	if (IsValid(this->Node))
	{
		this->Node->Event(InEvent);
	}
}
void UState::EventWithData(FName InEvent, UObject* Data)
{
	if (IsValid(this->Node))
	{
		this->Node->EventWithData(InEvent, Data);
	}
}

bool UState::HasNode() const
{
	return IsValid(this->Node);
}

bool UState::IsActive() const
{
	return this->GetMachine()->IsActiveState(this);
}

UObject* UState::GetOwner() const
{
	return this->OwnerMachine->GetOwner();
}

AActor* UState::GetActorOwner() const
{
	return this->OwnerMachine->GetActorOwner();
}

bool UState::HasPipedData() const
{
	return IsValid(this->Node) ? this->Node->HasPipedData() : false;
}

UObject* UState::GetPipedData() const
{
	return this->Node->GetPipedData();
}

void UState::GetTransitionEvents(TSet<FName>& Events) const
{
	for (const auto& Pair : this->Transitions)
	{
		Events.Add(Pair.Key);
	}
}

bool UState::GetDestination(FName InEvent, FDestinationResult& Result)
{
	this->Event_Inner(InEvent);
	if (!this->Transitions.Contains(InEvent)) { return false; }

	Result.Destination = NAME_None;
	bool Transition = false;

	if (auto Destination = this->Transitions.Find(InEvent))
	{
		for (const auto& Dest : Destination->Destinations)
		{
			if (Dest.Value.Condition->Check())
			{
				Result.Destination = Dest.Key;
				Result.Condition = Dest.Value.Condition;
				Transition = true;
				break;
			}
		}
	}

	return Transition;
}

bool UState::GetDataDestination(FName InEvent, UObject* Data, FDestinationResult& Result)
{
	this->EventWithData_Inner(InEvent, Data);
	if (!this->Transitions.Contains(InEvent)) { return false; }

	Result.Destination = NAME_None;
	bool Transition = false;

	if (auto Destination = this->Transitions.Find(InEvent))
	{
		for (const auto& Dest : Destination->Destinations)
		{
			if (Dest.Value.DataCondition->Check(Data))
			{
				Result.Destination = Dest.Key;
				Result.Condition = Dest.Value.DataCondition;
				Transition = true;
				break;
			}
		}
	}

	return Transition;
}

void UState::Event_Inner(FName InEvent) const
{
	for (const auto& Transition : this->Transitions)
	{
		for (const auto& Destination : Transition.Value.Destinations)
		{
			Destination.Value.Condition->Event(InEvent);
		}
	}
}

void UState::EventWithData_Inner(FName InEvent, UObject* Data) const
{
	for (const auto& Transition : this->Transitions)
	{
		for (const auto& Destination : Transition.Value.Destinations)
		{
			Destination.Value.Condition->EventWithData(InEvent, Data);
		}
	}
}

void FTransitionData::Initialize(UStateMachine* Owner, FName ISource, FName IDestination)
{
	this->Validate();

	this->Condition->Initialize(Owner, ISource, IDestination);
	this->DataCondition->Initialize(Owner, ISource, IDestination);
}

void FTransitionData::Validate()
{
	if (!this->Condition)
	{
		this->Condition = UTrueTransitionCondition::GetStaticTransition();
	}

	if (!this->DataCondition)
	{
		this->DataCondition = UTrueTransitionDataCondition::GetStaticTransition();
	}
}

void FTransitionData::Copy(UObject* NewOwner)
{
	if (this->Condition && !(this->Condition->GetOuter() == NewOwner))
	{
		this->Condition = DuplicateObject<UTransitionCondition>(this->Condition, NewOwner);
	}

	if (this->DataCondition && !(this->DataCondition->GetOuter() == NewOwner))
	{
		this->DataCondition = DuplicateObject<UTransitionDataCondition>(this->DataCondition, NewOwner);
	}
}

void FTransitionDataSet::Initialize(UStateMachine* Owner, FName ISource)
{
	for (auto& Dest : this->Destinations)
	{
		Dest.Value.Initialize(Owner, ISource, Dest.Key);
	}
}

void FTransitionDataSet::Copy(UObject* NewOwner)
{
	for (auto& Dest : this->Destinations)
	{
		Dest.Value.Copy(NewOwner);
	}
}

#undef LOCTEXT_NAMESPACE