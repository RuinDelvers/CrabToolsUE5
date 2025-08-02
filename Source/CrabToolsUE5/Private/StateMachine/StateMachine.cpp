#include "StateMachine/StateMachine.h"

#include "Logging/StructuredLog.h"
#include "Algo/Reverse.h"
#include "StateMachine/StateMachineBlueprintGeneratedClass.h"
#include "StateMachine/ArrayNode.h"
#include "StateMachine/Utils.h"
#include "StateMachine/Logging.h"
#include "StateMachine/IStateMachineLike.h"
#include "StateMachine/DataStructures.h"
#include "StateMachine/Transitions/BaseTransitions.h"
#include "Utils/UtilsLibrary.h"

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
	
		// Shared nodes always exist, and should be initialize from the beginning.
		for (auto& Node : this->SharedNodes)
		{
			Node.Value->Initialize(this);
		}		

		#if STATEMACHINE_DEBUG_DATA
			this->bWasInitialized = true;
		#endif


		this->UpdateState(this->GetStartState());
	}
	else
	{
		UE_LOG(LogStateMachine, Error, TEXT("Invalid Owner passed to Initialize in %s"), *this->GetClass()->GetName());
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

UState* UStateMachine::MakeState(FName StateName)
{
	auto State = NewObject<UState>(this);
	this->Graph.Add(StateName, State);
	return State;
}

void UStateMachine::UpdateState(FName Name)
{
	#if STATEMACHINE_DEBUG_DATA
		if (!this->bWasInitialized)
		{
			UE_LOG(LogStateMachine, Error, TEXT("State Machine %s was not initialized when calling UpdateState"),
				*this->GetName());
			return;
		}
	#endif

	if (Name != this->CurrentStateName)
	{
		auto CurrentState = this->GetCurrentState();

		// We check to see if there is data that needs to be piped.
		if (CurrentState->Node && CurrentState->Node->HasPipedData())
		{
			this->UpdateStateWithData(Name, CurrentState->Node->GetPipedData(), false);
		}
		else
		{

			this->TransitionIdentifier.EnterTransition();
			this->bIsTransitioning = true;

			FStateChangedEventData StateChangedData;

			StateChangedData.StateMachine = this;
			StateChangedData.From = this->CurrentStateName;
			StateChangedData.To = Name;


			auto OldState = this->CurrentStateName;

			if (CurrentState)
			{
				CurrentState->Exit();
				StateChangedData.FromState = CurrentState;
			}


			this->CurrentStateName = Name;
			this->PushStateToStack(Name);
			CurrentState = this->GetCurrentState();

			if (CurrentState)
			{
				StateChangedData.ToState = CurrentState;
				CurrentState->Enter();
			}

			this->StateChanged(StateChangedData);
			this->OnStateChanged.Broadcast(StateChangedData);

			this->bIsTransitioning = false;

			if (CurrentState)
			{
				CurrentState->Node->PostTransition();
				this->UpdateTickRequirements(CurrentState->Node->RequiresTick());
			}

			this->PostTransition();
			this->OnTransitionFinished.Broadcast(this);
		}
	}
}

void UStateMachine::UpdateStateWithData(FName Name, UObject* Data, bool UsePiped)
{
	#if STATEMACHINE_DEBUG_DATA
		if (!this->bWasInitialized)
		{
			UE_LOG(LogStateMachine, Error, TEXT("State Machine %s was not initialized when calling UpdateStateWithData"),
				*this->GetName());
			return;
		}
	#endif
	if (Name != this->CurrentStateName)
	{
		auto CurrentState = this->GetCurrentState();
		auto OldState = this->CurrentStateName;

		if (UsePiped && CurrentState->Node && CurrentState->Node->HasPipedData())
		{
			auto NewData = NewObject<UArrayNodeData>(this);

			NewData->AddData(Data);
			NewData->AddData(CurrentState->Node->GetPipedData());

			Data = NewData;
		}

		this->TransitionIdentifier.EnterTransition();
		this->bIsTransitioning = true;

		FStateChangedEventData StateChangedData;

		StateChangedData.StateMachine = this;
		StateChangedData.From = this->CurrentStateName;
		StateChangedData.To = Name;		
		

		if (CurrentState)
		{
			StateChangedData.FromState = CurrentState;
			CurrentState->ExitWithData(Data);
		}

		this->CurrentStateName = Name;
		this->PushStateToStack(Name);
		CurrentState = this->GetCurrentState();

		if (CurrentState)
		{
			StateChangedData.ToState = CurrentState;
			CurrentState->EnterWithData(Data);
		}

		this->StateChanged(StateChangedData);
		this->OnStateChanged.Broadcast(StateChangedData);
		this->bIsTransitioning = false;

		if (CurrentState)
		{
			CurrentState->Node->PostTransition();
			this->UpdateTickRequirements(CurrentState->Node->RequiresTick());
		}

		this->PostTransition();
		this->OnTransitionFinished.Broadcast(this);
	}
}

void UStateMachine::Tick(float DeltaTime) {
	auto State = this->GetCurrentState();

	if (State && State->Node) {
		State->Node->Tick(DeltaTime);
	}
}


void UStateMachine::Reset() {
	this->UpdateState(this->GetStartState());
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

void UStateMachine::SendEvent(FName EName)
{
	#if STATEMACHINE_DEBUG_DATA
		if (!this->bWasInitialized)
		{
			UE_LOG(LogStateMachine, Error, TEXT("State Machine %s was not initialized when calling SendEvent"),
				*this->GetName());
			return;
		}
	#endif

	if (this->bIsTransitioning) { return; }

	#if STATEMACHINE_DEBUG_DATA
		FStateMachineDebugDataFrame Frame;

		Frame.Event = EName;
		Frame.StartState = this->CurrentStateName;
		Frame.Time = this->GetWorld()->GetTimeSeconds();
	#endif

	auto CurrentState = this->GetCurrentState();

	if (CurrentState)
	{
		auto TID = this->GetStateID();

		// First we check if there are any declarative events to handle for this state.
		if (CurrentState->Transitions.Contains(EName))
		{
			auto& TData = CurrentState->Transitions[EName];

			if (TData.Condition->Check())
			{
				this->UpdateState(TData.Destination);
				TData.Condition->OnTransitionTaken();

				#if STATEMACHINE_DEBUG_DATA
					Frame.EndState = this->CurrentStateName;
					this->DebugData.CurrentStateTime = Frame.Time;
				#endif

				return;
			}
		}
		
		if (CurrentState->Node) CurrentState->Node->Event(EName);
	}
}

void UStateMachine::SendEventWithData(FName EName, UObject* Data)
{
	#if STATEMACHINE_DEBUG_DATA
		if (!this->bWasInitialized)
		{
			UE_LOG(LogStateMachine, Error, TEXT("State Machine %s was not initialized when calling SendEventWithData"),
				*this->GetName());
			return;
		}
	#endif
	if (this->bIsTransitioning) { return; }

	#if STATEMACHINE_DEBUG_DATA
		FStateMachineDebugDataFrame Frame;

		Frame.Event = EName;
		Frame.StartState = this->CurrentStateName;
		Frame.Time = this->GetWorld()->GetTimeSeconds();
	#endif

	auto CurrentState = this->GetCurrentState();

	if (CurrentState)
	{

		// First we check if there are any declarative events to handle for this state.
		if (CurrentState->Transitions.Contains(EName))
		{
			auto& TData = CurrentState->Transitions[EName];

			if (TData.DataCondition->Check(Data))
			{
				this->UpdateStateWithData(TData.Destination, Data);
				TData.DataCondition->OnTransitionTaken(Data);

				#if STATEMACHINE_DEBUG_DATA
					Frame.EndState = this->CurrentStateName;
					this->DebugData.CurrentStateTime = Frame.Time;
				#endif

				return;
			}
		}

		if (CurrentState->Node) CurrentState->Node->EventWithData(EName, Data);
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
	if (auto State = this->GetCurrentState())
	{
		if (State->GetNode())
		{
			State->GetNode()->SetActive(bNewActive);
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

#endif // WITH_EDITOR

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

	for (const auto& States : this->Graph) {
		for (const auto& Event : States.Value->Transitions) {
			List.Add(Event.Key);
		}
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

UState* UStateMachine::MakeStateWithNode(FName StateName, UStateNode* Node)
{
	UState* Data = NewObject<UState>(this);
	Data->Node = Cast<UStateNode>(DuplicateObject(Node, this));
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
		if (Node->Node && Node->Node->IsA(Class.Get()->StaticClass()))
		{
			Branches = ESearchResult::Found;

			return Node->Node;
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

		if (BuiltState)
		{
			for (auto& tpairs : BuiltState->Transitions)
			{
				if (!IsValid(tpairs.Value.Condition))
				{
					tpairs.Value.Condition = UTrueTransitionCondition::GetStaticTransition();;
				}

				if (!IsValid(tpairs.Value.DataCondition))
				{
					tpairs.Value.DataCondition = UTrueTransitionDataCondition::GetStaticTransition();
				}

				tpairs.Value.Condition->Initialize(this);
				tpairs.Value.DataCondition->Initialize(this);
			}				
		}
		else
		{
			BuiltState = NewObject<UState>();			
		}

		if (IsValid(BuiltState))
		{
			BuiltState->Initialize(this);
		}

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

FSMPropertyReference UStateMachine::GetStateMachineProperty(FString& Address) const
{
	FString Base;
	FString Target;
	
	if (Address.Split(TEXT("/"), &Base, &Target))
	{
		FName BaseName(Base);

		if (auto SubM = this->SubMachines.Find(BaseName))
		{
			return SubM->Get()->GetStateMachineProperty(Address);
		}
		else if (Base == "..")
		{
			if (this->ParentMachine)
			{
				return this->ParentMachine->GetStateMachineProperty(Target);
			}
			else
			{
				return FSMPropertyReference();
			}
		}
		else
		{
			return FSMPropertyReference();
		}
	}
	else
	{
		return FSMPropertyReference
		{
			this->GetClass()->FindPropertyByName(FName(Address)),
			(UObject*) this
		};
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

void UStateMachine::PushStateToStack(FName EName)
{
	if (this->StateStack.Num() >= this->MaxPrevStateStackSize)
	{
		this->StateStack.RemoveNode(this->StateStack.GetHead());
	}

	this->StateStack.AddTail(EName);
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

#if WITH_EDITOR
TArray<FString> UStateMachine::GetPropertiesOptions(const FSMPropertySearch& SearchParam) const
{
	TArray<FString> Names;

	
		for (TFieldIterator<FProperty> FIT(this->GetClass(), EFieldIteratorFlags::IncludeSuper); FIT; ++FIT)
		{
			FProperty* f = *FIT;

			if (SearchParam.Matches(f))
			{
				Names.Add(f->GetName());
			}
		}

		for (auto& SubMachine : this->SubMachines)
		{
			for (TFieldIterator<FProperty> FIT(SubMachine.Value->GetClass(), EFieldIteratorFlags::IncludeSuper); FIT; ++FIT)
			{
				FProperty* f = *FIT;

				if (SearchParam.Matches(f))
				{
					FString Formatted = FString::Printf(TEXT("%s/%s"), *SubMachine.Key.ToString(), *f->GetName());
					Names.Add(Formatted);
				}
			}
		}

	return Names;
}
#endif //WITH_EDITOR

#pragma endregion

#pragma region NodeCode

UStateNode::UStateNode()
{

}

void UStateNode::Initialize(UStateMachine* POwner) {
	if (POwner)
	{
		this->Owner = POwner;
		this->InitNotifies();
		this->Initialize_Inner();
	}
	else
	{
		UE_LOG(LogStateMachine, Error, TEXT("Given owner for %s was null"), *this->GetName());
	}
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
			FName EventName(f->GetName().RightChop(11));

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

void UStateNode::Event(FName EName) {
	if (this->bActive) {
		if (auto Notify = this->EventNotifies.Find(EName))
		{
			Notify->Broadcast(EName);
		}

		this->Event_Inner(EName);
	}
}

UStateMachine* UStateNode::GetRootMachine() const
{
	return this->Owner->GetRootMachine();
}

void UStateNode::Event_Inner_Implementation(FName EName) {
	// Does Nothing by default.
}

void UStateNode::EventWithData(FName EName, UObject* Data) {
	if (this->bActive)
	{
		if (auto Notify = this->EventWithDataNotifies.Find(EName))
		{
			Notify->Broadcast(EName, Data);
		}

		this->EventWithData_Inner(EName, Data);
	}
}

void UStateNode::EventWithData_Inner_Implementation(FName EName, UObject* Data) {
	this->Event_Inner(EName);
}

bool UStateNode::DoesReferenceMachine(FName MachineName) const
{
	return this->DoesReferenceMachine_Inner(MachineName);
}

void UStateNode::SetOwner(UStateMachine* Parent) {
	this->Owner = Parent;
}

void UStateNode::SetActive(bool bNewActive)
{
	this->bActive = bNewActive;
	this->SetActive_Inner(bNewActive);
}

void UStateNode::Tick(float DeltaTime) {
	if (this->bActive) {
		this->Tick_Inner(DeltaTime);
	}
}

void UStateNode::PostTransition()
{
	if (this->bActive)
	{
		this->PostTransition_Inner();
	}
}

void UStateNode::Exit()
{
	this->SetActive(false);
	this->Exit_Inner();
}

void UStateNode::ExitWithData(UObject* Data)
{
	this->SetActive(false);
	this->ExitWithData_Inner(Data);
}

void UStateNode::ExitWithData_Inner_Implementation(UObject* Data)
{
	this->Exit_Inner();
}

void UStateNode::Enter()
{
	this->SetActive(true);
	this->Enter_Inner();
}

void UStateNode::EnterWithData(UObject* Data)
{
	this->SetActive(true);
	this->EnterWithData_Inner(Data);
}

void UStateNode::EnterWithData_Inner_Implementation(UObject* Data)
{
	this->Enter_Inner();
}

void UStateNode::DeleteEvent_Implementation(FName Event)
{
	for (TFieldIterator<FStructProperty> FIT(this->GetClass(), EFieldIteratorFlags::IncludeSuper); FIT; ++FIT)
	{
		FStructProperty* f = *FIT;

		if (f->Struct == FEventSlot::StaticStruct())
		{
			auto Value = f->ContainerPtrToValuePtr<FEventSlot>(this);
			if (Value->GetEvent() == Event)
			{
				Value->SetEvent(NAME_None);
			}
		}
	}
}

void UStateNode::RenameEvent_Implementation(FName From, FName To)
{
	for (TFieldIterator<FStructProperty> FIT(this->GetClass(), EFieldIteratorFlags::IncludeSuper); FIT; ++FIT)
	{
		FStructProperty* f = *FIT;

		if (f->Struct == FEventSlot::StaticStruct())
		{
			auto Value = f->ContainerPtrToValuePtr<FEventSlot>(this);

			if (Value->GetEvent() == From)
			{
				Value->SetEvent(To);
			}
		}
	}
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

				if (f->Struct == FEventSlot::StaticStruct())
				{
					FEventSlot Value;
					f->GetValue_InContainer(this, &Value);

					auto Options = SLike->GetEventOptions();

					if (!(Value.IsNone() || Options.Contains(Value.GetEvent())))
					{
						FString Msg = FString::Printf(TEXT("Could not find Event for slot %s: %s. EName = %s"),
							*f->GetName(),
							*this->GetName(),
							*FName(Value).ToString());
						Context.Error(Msg, this);

						bErrorFree = false;
					}

				}
				else if (f->Struct == FSubMachineSlot::StaticStruct())
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
				TObjectPtr<UObject> Value = nullptr;
				FObjectProperty* f = CastField<FObjectProperty>(*FIT);
				f->GetValue_InContainer(this, &Value);

				if (auto Prop = Cast<UStateMachineProperty>(Value))
				{
					Prop->Verify(Context);
				}
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

void UStateNode::EmitEvent(FName EName)
{
	if (this->Active())
	{
		this->GetMachine()->SendEvent(EName);
	}
}

void UStateNode::EmitEventSlot(const FEventSlot& ESlot)
{ 
	this->GetMachine()->SendEvent(ESlot);
}

void UStateNode::EmitEventWithData(FName EName, UObject* Data) {
	this->GetMachine()->SendEventWithData(EName, Data);
}

void UStateNode::EmitEventSlotWithData(const FEventSlot& ESlot, UObject* Data)
{
	this->GetMachine()->SendEventWithData(ESlot, Data);
}

#if WITH_EDITOR
TArray<FString> UStateNode::GetPropertyOptions(const FSMPropertySearch& Params) const
{
	TArray<FString> Props;

	if (auto Outer = UtilsFunctions::GetOuterAs<IStateMachineLike>(this))
	{
		Props.Append(Outer->GetPropertiesOptions(Params));
	}

	if (auto Outer = UtilsFunctions::GetOuterAs<IStateLike>(this))
	{
		Props.Append(Outer->GetPropertiesOptions(Params));
	}

	Props.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Props;
}

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
			FName EventName(f->GetName().RightChop(11));

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
	this->Transitions.Append(Data->Transitions);
}

void UState::AppendCopy(UState* Data)
{
	this->AppendNodeCopy(Data->Node);
	this->Transitions.Append(Data->Transitions);
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

UObject* UTransitionCondition::GetOwner() const { return this->OwnerMachine->GetOwner(); }
AActor*  UTransitionCondition::GetActorOwner() const { return this->OwnerMachine->GetActorOwner(); }
UObject* UTransitionDataCondition::GetOwner() const { return this->OwnerMachine->GetOwner(); }
AActor* UTransitionDataCondition::GetActorOwner() const { return this->OwnerMachine->GetActorOwner(); }

void UTransitionCondition::Initialize(UStateMachine* NewOwner)
{
	this->OwnerMachine = NewOwner;
	this->Initialize_Inner();
}

void UTransitionDataCondition::Initialize(UStateMachine* NewOwner)
{
	this->OwnerMachine = NewOwner;
	this->Initialize_Inner();
}


void UState::Initialize(UStateMachine* POwner)
{
	this->OwnerMachine = POwner;

	if (IsValid(this->Node))
	{
		this->Node->Initialize(POwner);
	}
}

void UState::Enter()
{
	this->Enter_Inner();
	if (IsValid(this->Node))
	{
		this->Node->Enter();
	}
}

void UState::EnterWithData(UObject* Data)
{
	this->EnterWithData_Inner(Data);
	if (IsValid(this->Node))
	{
		this->Node->EnterWithData(Data);
	}
}

void UState::Exit()
{
	this->Exit_Inner();
	if (IsValid(this->Node))
	{
		this->Node->Exit();
	}
}

void UState::ExitWithData(UObject* Data)
{
	this->ExitWithData_Inner(Data);
	if (IsValid(this->Node))
	{
		this->Node->ExitWithData(Data);
	}
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

FSMPropertyReference UState::GetStateMachineProperty(FString& Address) const
{
	return FSMPropertyReference
	{
		this->GetClass()->FindPropertyByName(FName(Address)),
		(UObject*)this
	};
}

#if WITH_EDITOR
TArray<FString> UState::GetPropertiesOptions(const FSMPropertySearch& SearchParam) const
{
	TArray<FString> Names;


	for (TFieldIterator<FProperty> FIT(this->GetClass(), EFieldIteratorFlags::IncludeSuper); FIT; ++FIT)
	{
		FProperty* f = *FIT;

		if (SearchParam.Matches(f))
		{
			Names.Add(FString::Printf(TEXT("::State::%s"), *f->GetName()));
		}
	}

	return Names;
}
#endif

#if STATEMACHINE_DEBUG_DATA
bool FStateMachineDebugDataFrame::DidTransition()
{
	return !this->EndState.IsNone() && this->EndState != this->StartState;
}
#endif

UStateNode* UStateMachineProperty::GetNode() const
{
	return UtilsFunctions::GetOuterAs<UStateNode>(this);
}

TArray<FString> UStateMachineProperty::DoPropertySearch() const
{
	#if WITH_EDITOR
		return this->GetNode()->GetPropertyOptions(this->Params);
	#else
		return {};
	#endif
}

const FSMPropertyReference& UStateMachineProperty::GetProperty()
{
	if (!this->bDidInit)
	{
		FString Address = this->Name.ToString();
		this->Ref = this->Params.GetProperty<UStateNode>(this->GetNode(), Address);
	}

	return this->Ref;
}

void UStateMachineProperty::Verify(FNodeVerificationContext& Context)
{
	auto Props = this->DoPropertySearch();

	if (!Props.Contains(this->Name))
	{
		//FString Msg = FString::Printf(
		//	TEXT("Failed to find property %s in Node %s"),
		//	*this->Name.ToString(),
		//	*this->GetNode()->GetName());
		//Context.Error(Msg, this);
	}
}