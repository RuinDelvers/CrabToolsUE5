#include "StateMachine/EdGraph/EdStateGraph.h"

#include "StateMachine/EdGraph/EdBaseNode.h"
#include "StateMachine/EdGraph/EdStartStateNode.h"
#include "StateMachine/EdGraph/EdAliasNode.h"
#include "StateMachine/EdGraph/EdEventObject.h"
#include "StateMachine/EdGraph/EdTransition.h"
#include "StateMachine/StateMachineBlueprint.h"
#include "StateMachine/StateMachineBlueprintGeneratedClass.h"
#include "KismetCompiler.h"
#include "Engine/DataTable.h"
#include "Utils/PropertyManagement.h"
#include "StateMachine/DataStructures.h"

#define LOCTEXT_NAMESPACE "EdStateGraph"

constexpr char DEFAULT_NODE_NAME[] = "NewState";
constexpr char DEFAULT_EVENT_NAME[] = "NewEvent";

UEdStateGraph::UEdStateGraph()
: Accessibility(EStateMachineAccessibility::PRIVATE),
	DefaultStateClass(nullptr)
{
	this->MachineArchetype = this->CreateDefaultSubobject<UStateMachine>(TEXT("Default Machine"));
	this->DefaultStateClass = UState::StaticClass();
}

void UEdStateGraph::Initialize(UStateMachineBlueprint* BP)
{

}

void UEdStateGraph::SetDebugMachine(UStateMachine* Machine)
{
	if (this->DebugStateMachine)
	{
		this->DebugStateMachine->OnStateChanged.RemoveAll(this);

		for (const auto StateNode : this->GetStates())
		{
			StateNode->SetDebugObject(nullptr);
		}
	}

	if (IsValid(Machine))
	{
		Machine->OnStateChanged.AddDynamic(this, &UEdStateGraph::OnDebugStateChanged);

		if (auto StateNode = this->GetStateNodeByName(Machine->GetCurrentStateName()))
		{
			StateNode->SetDebugObject(Machine->GetCurrentStateData());
		}
	}
}

void UEdStateGraph::NotifyGraphChanged(const FEdGraphEditAction& Action)
{
	Super::NotifyGraphChanged(Action);

	if (Action.Action & EEdGraphActionType::GRAPHACTION_AddNode)
	{
		for (auto Node : Action.Nodes)
		{
			UEdStateNode* CastNode = Cast<UEdStateNode>(const_cast<UEdGraphNode*>(Node));

			if (CastNode)
			{
				this->Events.OnStateAdded.Broadcast(CastNode);
			}			
		}
	}
	else if (Action.Action & EEdGraphActionType::GRAPHACTION_RemoveNode)
	{
		for (auto Node : Action.Nodes)
		{
			UEdStateNode* CastNode = Cast<UEdStateNode>(const_cast<UEdGraphNode*>(Node));

			if (CastNode)
			{
				CastNode->Events.OnNodeDeleted.Broadcast();
			}
		}
	}

	this->Modify();
}

FName UEdStateGraph::GetNewStateName()
{
	FString DefaultName(DEFAULT_NODE_NAME);
	int index = 0;

	for (auto& Node : this->Nodes)
	{
		if (UEdStateNode* CastNode = Cast<UEdStateNode>(Node))
		{
			FString NodeName = CastNode->GetStateName().ToString();

			if (NodeName.StartsWith(DEFAULT_NODE_NAME))
			{
				FString EndName = NodeName.RightChop(DefaultName.Len());

				if (EndName.IsNumeric())
				{
					int check = FCString::Atoi(*EndName);

					if (check >= index)
					{
						index = check + 1;
					}
				}
			}
		}
	}

	DefaultName.AppendInt(index);

	return FName(DefaultName);
}

UEdStateNode* UEdStateGraph::GetStateNodeByName(FName Name) const
{
	for (const auto& Node : this->Nodes)
	{
		if (auto StateNode = Cast<UEdStateNode>(Node))
		{
			if (StateNode->GetStateName() == Name)
			{
				return StateNode;
			}
		}
	}

	return nullptr;
}

UEdStartStateNode* UEdStateGraph::GetStartNode() const
{
	TArray<UEdStartStateNode*> PossibleStarts;
	this->GetNodesOfClass<UEdStartStateNode>(PossibleStarts);

	// Should never be null.
	return PossibleStarts[0];
}

FName UEdStateGraph::GetClassPrefix() const
{
	if (this->IsMainGraph())
	{
		return this->GetBlueprintOwner()->GeneratedClass->GetFName();
	}
	else
	{
		FString Total;

		Total.Append(this->GetBlueprintOwner()->GeneratedClass->GetName());
		Total.Append("::");
		Total.Append(this->GetName());

		return FName(Total);
	}
}

TArray<UEdBaseNode*> UEdStateGraph::GetDestinations(UEdBaseNode* Node) const
{
	TArray<UEdBaseNode*> Destinations;

	TArray<UEdTransition*> Transitions;
	this->GetNodesOfClass<UEdTransition>(Transitions);

	for (auto Trans : Transitions)
	{
		if (Trans->GetStartNode() == Node)
		{
			Destinations.Add(Trans->GetEndNode());
		}
	}

	return Destinations;
}

FName UEdStateGraph::GetParentDefinedStartState() const
{
	FName Found;

	if (auto Parent = this->GetBlueprintOwner()->GetStateMachineGeneratedClass()->GetParent())
	{
		auto Data = Parent->GetMachineArchetypeData(this->GetGraphName());

		if (Data->bCanOverrideStart)
		{
			FName LocalStart = this->GetLocallyDefinedStartState();

			if (LocalStart.IsNone())
			{
				Found = Data->GetArchetype()->StartState;
			}
			else
			{
				Found = LocalStart;
			}
		}
		else
		{
			Found = Data->GetArchetype()->StartState;
		}	
	}

	return Found;
}

FName UEdStateGraph::GetLocallyDefinedStartState() const
{
	FName Found;

	TArray<UEdTransition*> Transitions;
	this->GetNodesOfClass<UEdTransition>(Transitions);

	for (auto Trans : Transitions)
	{
		if (auto Start = Cast<UEdStartStateNode>(Trans->GetStartNode()))
		{
			if (auto Dest = Cast<UEdStateNode>(Trans->GetEndNode()))
			{
				Found = Dest->GetStateName();
			}
		}
	}

	return Found;
}

FName UEdStateGraph::GetStartStateName() const
{
	if (this->GraphType == EStateMachineGraphType::MAIN_GRAPH || this->GraphType == EStateMachineGraphType::SUB_GRAPH)
	{
		return this->GetLocallyDefinedStartState();
	}
	else
	{
		return this->GetParentDefinedStartState();
	}
}

bool UEdStateGraph::HasEvent(FName EName) const
{
	bool bHasEvent = false;
	auto Ev = this->EventObjects.FindByPredicate([&](const UEdEventObject* Ev) { return Ev->GetEventName() == EName; });

	if (Ev)
	{
		bHasEvent = true;
	}
	else
	{
		bHasEvent = this->DoesEmitterHaveEvent(EName) || this->GetBlueprintOwner()->HasEvent(EName);

		if (!this->IsMainGraph())
		{
			UStateMachineBlueprintGeneratedClass* BPGC = nullptr;

			if (this->GraphType == EStateMachineGraphType::EXTENDED_GRAPH)
			{
				BPGC = Cast<UStateMachineBlueprintGeneratedClass>(this->MachineArchetypeOverride.Value->GetClass());
			}
			else if (this->GraphType == EStateMachineGraphType::EXTENDED_GRAPH)
			{
				BPGC = Cast<UStateMachineBlueprintGeneratedClass>(this->MachineArchetype->GetClass());
			}

			if (BPGC != nullptr)
			{
				for (auto& IFace : BPGC->Interfaces)
				{
					if (IFace->HasEvent(EName))
					{
						bHasEvent = true;
						break;
					}
				}
			}
		}
	}	

	return bHasEvent;;
}

bool UEdStateGraph::DoesEmitterHaveEvent(FName EName) const
{
	for (auto& Emitter : this->EventEmitters)
	{
		if (Emitter->GetEmittedEvents().Contains(EName))
		{
			return true;
		}
	}

	return false;
}




void UEdStateGraph::ClearDelegates()
{
	this->Events.OnNodeSelected.Clear();
	this->Events.OnEventCreated.Clear();

	for (auto& Node : this->Nodes)
	{
		if (auto StateNode = Cast<UEdBaseStateNode>(Node))
		{
			StateNode->ClearEvents();
		}
	}
}

UEdEventObject* UEdStateGraph::CreateEvent()
{
	FString DefaultName(DEFAULT_EVENT_NAME);
	int index = 0;

	for (auto& Event : this->EventObjects)
	{
		FString EventName = Event->GetEventName().ToString();

		if (EventName.StartsWith(DEFAULT_EVENT_NAME))
		{
			FString EndName = EventName.RightChop(DefaultName.Len());

			if (EndName.IsNumeric())
			{
				int check = FCString::Atoi(*EndName);

				if (check >= index)
				{
					index = check + 1;
				}
			}
		}
	}

	DefaultName.AppendInt(index);

	const FScopedTransaction Transaction(LOCTEXT("CreateEvent", "Create Event"));
	this->Modify();

	auto NewName = FName(DefaultName);
	auto NewEventObject = NewObject<UEdEventObject>(this);
	NewEventObject->SetName(NewName);

	this->EventObjects.Add(NewEventObject);
	this->Events.OnEventCreated.Broadcast(NewEventObject);	

	return NewEventObject;
}

bool UEdStateGraph::IsEventNameAvailable(FName Name) const
{
	if (this->GetBlueprintOwner()->IsEventNameAvailable(Name))
	{
		for (auto& EventObj : this->EventObjects)
		{
			if (EventObj->GetEventName() == Name)
			{
				return false;
			}
		}
	}
	else
	{
		return false;
	}

	return true;
}

bool UEdStateGraph::IsStateNameAvailable(FName Name) const
{
	for (const auto& Node : this->Nodes)
	{
		if (UEdStateNode* CastNode = Cast<UEdStateNode>(Node))
		{
			if (CastNode->GetStateName() == Name)
			{
				return false;
			}
		}
	}

	return true;
}

void UEdStateGraph::Verify(FNodeVerificationContext& Context, UStateMachineInterface* IFace) const
{
	auto PublicStates = this->GetAllPublicStates();

	for (auto& StateName : IFace->GetStates())
	{
		if (!PublicStates.Contains(StateName))
		{
			FString ErrorMessage = FString::Printf(
				TEXT("Graph %s does not contain public state %s as needed for interface %s"),
				*this->GetDisplayName(),
				*StateName.ToString(),
				*IFace->GetName());

			Context.Error(ErrorMessage, this);
		}
	}
}

FName UEdStateGraph::RenameEvent(UEdEventObject* EventObj, FName To)
{
	if (this->IsEventNameAvailable(To))
	{
		EventObj->SetName(To);
		return To;
	}
	else
	{
		return EventObj->GetEventName();
	}
}

TArray<UEdStateNode*> UEdStateGraph::GetStates() const
{
	TArray<UEdStateNode*> StateNodes;
	this->GetNodesOfClass<UEdStateNode>(StateNodes);

	return StateNodes;
}

TArray<UEdTransition*> UEdStateGraph::GetTransitions() const
{
	TArray<UEdTransition*> Transitions;
	this->GetNodesOfClass<UEdTransition>(Transitions);

	return Transitions;
}

TArray<UEdTransition*> UEdStateGraph::GetExitTransitions(UEdStateNode* Start) const
{
	TArray<UEdTransition*> Transitions;
	this->GetNodesOfClass<UEdTransition>(Transitions);

	auto Pred = [&](const UEdTransition* A) -> bool {
		if (auto Alias = Cast<UEdAliasNode>(A->GetStartNode()))
		{
			return Alias->Matches(Start);
		}
		return A->GetStartNode() == Start;
	};

	return Transitions.FilterByPredicate(Pred);
}

FStateMachineArchetypeData UEdStateGraph::CompileStateMachine(FNodeVerificationContext& Context)
{
	if (this->UpdateOverrideData())
	{
		FString ErrorMessage = FString::Printf(
			TEXT("Subgraph %s::%s overriden statemachine is out of sync with its parent and has been modified."),
			*this->GetBlueprintOwner()->GetName(),
			*this->GetName());
		Context.Warning(ErrorMessage, this);
	}

	this->VerifyMachineArchetypes(Context);

	FStateMachineArchetypeData Data;

	Data.bCanOverrideStart = this->GetStartNode()->CanOverride();

	if (!this->IsMainGraph())
	{
		if (this->GraphType == EStateMachineGraphType::EXTENDED_GRAPH)
		{
			if (this->OverridenMachine.IsNone() || this->MachineArchetypeOverride.Value == nullptr)
			{
				FString ErrorMessage = FString::Printf(
					TEXT("Subgraph %s specified as extended, but no parent submachine specified to be extended."),
					*this->GetName());
				Context.Error(ErrorMessage, this);

				return nullptr;
			}
			Data.SetArchetype(DuplicateObject<UStateMachine>(this->MachineArchetypeOverride.Value.Get(), Context.GetOuter()));
		}
		else
		{
			Data.SetArchetype(DuplicateObject<UStateMachine>(this->MachineArchetype, Context.GetOuter()));
		}
		
		Data.bInstanceEditable = this->bInstanceEditable;
	}
	else
	{
		Data.SetArchetype(NewObject<UStateMachine>(Context.GetOuter()));
	}
	
	Data.bIsVariable = this->bIsVariable;
	Data.Accessibility = this->Accessibility;

	for (auto& State : this->GetStates())
	{
		FStateArchetypeData BuiltState = State->CompileState(Context, Context.GetOuter());

		for (auto Transition : this->GetExitTransitions(State))
		{
			auto TData = Transition->GetTransitionData(Context);

			for (auto& Values : TData)
			{
				BuiltState.GetArchetype()->AddTransition(Values.Key, Values.Value);
			}
		}

		Data.AddStateData(State->GetStateName(), BuiltState);
	}

	Data.GetArchetype()->StartState = this->GetStartStateName();
	

	return Data;
}

bool UEdStateGraph::IsMainGraph() const
{
	if (auto BP = this->GetBlueprintOwner())
	{
		return BP->IsMainGraph(this);
	}

	return false;
}

FString UEdStateGraph::GetDisplayName() const
{
	if (this->IsMainGraph())
	{
		return "Main";
	}
	else
	{
		return this->GetName();
	}
}

bool UEdStateGraph::CanOverrideStart() const
{
	switch (this->GraphType)
	{
		case EStateMachineGraphType::MAIN_GRAPH: return true;
		case EStateMachineGraphType::SUB_GRAPH: return true;
		case EStateMachineGraphType::EXTENDED_GRAPH:
			if (auto BPGC = this->GetBlueprintOwner()->GetStateMachineGeneratedClass()->GetParent())
			{
				if (this->OverridenMachine.IsNone()) { return false; }

				if (auto Arch = BPGC->GetMachineArchetypeData(this->GetFName()))
				{
					return Arch->bCanOverrideStart;
				}
				else
				{
					UE_LOG(LogStateMachine, Error, TEXT("Extended graph cannot find parent submachine: %s::%s"),
						*this->GetBlueprintOwner()->GetName(),
						*this->GetName());

					return false;
				}
			}
			else
			{
				UE_LOG(LogStateMachine, Error, TEXT("Extended graph cannot find parent submachine: %s::%s"),
					*this->GetBlueprintOwner()->GetName(),
					*this->GetName());
				return false;
			}
		default: return false;
	}
}

bool UEdStateGraph::IsVariable() const 
{
	return StateMachineAccessibility::IsPublic(this->Accessibility) && this->bIsVariable;
}

UStateMachineBlueprint* UEdStateGraph::GetBlueprintOwner() const
{
	return Cast<UStateMachineBlueprint>(this->GetOuter());
}

void UEdStateGraph::Select()
{
	this->GetBlueprintOwner()->SelectGraph(this);
	this->GetBlueprintOwner()->Events.OnObjectInspected.Broadcast(this);
}

void UEdStateGraph::Inspect()
{
	this->GetBlueprintOwner()->Events.OnObjectInspected.Broadcast(this);
}

TSet<FName> UEdStateGraph::GetAllPublicStates() const
{
	TSet<FName> Names;

	for (auto& Node : this->Nodes)
	{
		if (auto StateNode = Cast<UEdStateNode>(Node))
		{
			Names.Add(StateNode->GetStateName());
		}
	}

	if (this->IsMainGraph())
	{
		if (auto Parent = this->GetBlueprintOwner()->GetStateMachineGeneratedClass()->GetParent())
		{
			Parent->AppendPublicStateNames(Names);
		}
	}
	else
	{
		if (this->GraphType == EStateMachineGraphType::SUB_GRAPH)
		{
			if (auto BPGC = Cast<UStateMachineBlueprintGeneratedClass>(this->MachineArchetype->GetClass()))
			{
				BPGC->AppendPublicStateNames(Names);
			}
		}
		else if (this->GraphType == EStateMachineGraphType::EXTENDED_GRAPH)
		{
			if (auto BPGC = Cast<UStateMachineBlueprintGeneratedClass>(this->MachineArchetypeOverride.Value->GetClass()))
			{
				BPGC->AppendPublicStateNames(Names);
			}
		}
	}

	return Names;
}

TArray<FString> UEdStateGraph::GetStateOptions(const UObject* Asker) const
{
	TArray<FString> Names;

	for (auto& Node : this->Nodes)
	{
		if (auto StateNode = Cast<UEdStateNode>(Node))
		{
			Names.Add(StateNode->GetStateName().ToString());
		}
	}

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}

TArray<FString> UEdStateGraph::GetEventOptions() const
{
	TArray<FString> Names;

	for (auto& Ev : this->EventObjects)
	{
		Names.Add(Ev->GetEventName().ToString());
	}

	this->AppendEmitterEvents(Names);
	this->GetBlueprintOwner()->AppendInterfaceEvents(Names);

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}

void UEdStateGraph::AppendEmitterEvents(TArray<FString>& Names) const
{
	for (auto& Emitter : this->EventEmitters)
	{		
		for (auto& EName : Emitter->GetEmittedEvents())
		{
			Names.Add(EName.ToString());
		}
	}
}

UStateMachine* UEdStateGraph::GetSourceMachine() const
{
	if (this->IsMainGraph())
	{
		return Cast<UStateMachine>(this->GetBlueprintOwner()->GetStateMachineGeneratedClass()->GetDefaultObject());
	}
	else
	{
		if (this->GraphType == EStateMachineGraphType::SUB_GRAPH)
		{
			return this->MachineArchetype;
		}
		else if (this->GraphType == EStateMachineGraphType::EXTENDED_GRAPH)
		{
			return this->MachineArchetypeOverride.Value;
		}
	}

	return nullptr;
}

TArray<FString> UEdStateGraph::GetConditionOptions() const
{
	TArray<FString> Names;

	UClass* ClassBase = nullptr;

	if (this->IsMainGraph())
	{
		ClassBase = this->GetBlueprintOwner()->GeneratedClass;
	}
	else
	{
		if (this->MachineArchetype)
		{
			ClassBase = this->MachineArchetype->GetClass();
		}
		else
		{
			return Names;
		}
	}

	// Find the example function to check against to find others of the same signature.
	auto FnArchetype = ClassBase->FindFunctionByName("TrueCondition");

	for (TFieldIterator<UFunction> FIT(ClassBase, EFieldIteratorFlags::IncludeSuper); FIT; ++FIT)
	{
		UFunction* f = *FIT;

		if (f->IsSignatureCompatibleWith(FnArchetype))
		{
			Names.Add(f->GetName());
		}
	}

	// If it's a submachine, allow binding to parent functions.
	if (!this->IsMainGraph())
	{
		ClassBase = this->GetBlueprintOwner()->GeneratedClass;
		FnArchetype = ClassBase->FindFunctionByName("TrueCondition");

		for (TFieldIterator<UFunction> FIT(ClassBase, EFieldIteratorFlags::IncludeSuper); FIT; ++FIT)
		{
			UFunction* f = *FIT;

			if (f->IsSignatureCompatibleWith(FnArchetype) && !f->HasMetaData("IsDefaultCondition"))
			{
				Names.Add("../" + f->GetName());
			}
		}
	}

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}

FName UEdStateGraph::RenameNode(UEdBaseStateNode* Node, FName NewName)
{

	if (this->IsStateNameAvailable(NewName))
	{
		FName OldName = Node->GetStateName();
		Node->SetStateName(NewName);

		return NewName;
	}

	return Node->GetStateName();
}

TArray<FString> UEdStateGraph::GetDataConditionOptions() const
{
	TArray<FString> Names;

	UClass* ClassBase = nullptr;

	if (this->IsMainGraph())
	{
		ClassBase = this->GetBlueprintOwner()->GeneratedClass;
	}
	else
	{
		if (this->MachineArchetype)
		{
			ClassBase = this->MachineArchetype->GetClass();
		}
		else
		{
			return Names;
		}
	}

	// Find the example function to check against to find others of the same signature.
	auto FnArchetype = ClassBase->FindFunctionByName("TrueDataCondition");

	for (TFieldIterator<UFunction> FIT(ClassBase, EFieldIteratorFlags::IncludeSuper); FIT; ++FIT)
	{
		UFunction* f = *FIT;

		if (f->IsSignatureCompatibleWith(FnArchetype))
		{
			Names.Add(f->GetName());
		}
	}

	// If it's a submachine, allow binding to parent functions.
	if (!this->IsMainGraph())
	{
		ClassBase = this->GetBlueprintOwner()->GeneratedClass;
		FnArchetype = ClassBase->FindFunctionByName("TrueDataCondition");

		for (TFieldIterator<UFunction> FIT(ClassBase, EFieldIteratorFlags::IncludeSuper); FIT; ++FIT)
		{
			UFunction* f = *FIT;

			if (f->IsSignatureCompatibleWith(FnArchetype) && !f->HasMetaData("IsDefaultCondition"))
			{
				Names.Add("../" + f->GetName());
			}
		}
	}

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}

TArray<FString> UEdStateGraph::GetMachineOptions() const
{
	TArray<FString> Names = this->GetBlueprintOwner()->GetMachineOptions();

	if (!this->IsMainGraph())
	{
		// Need to make sure the references are to the parent machine.
		TArray<FString> NewNames;

		for (auto& OldName : Names)
		{
			NewNames.Add("../" + OldName);
		}

		if (auto BPGC = Cast<UStateMachineBlueprintGeneratedClass>(this->MachineArchetype->GetClass()))
		{
			NewNames.Append(BPGC->GetChildAccessibleSubMachines());
		}

		Names = NewNames;
	}

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}

TArray<FString> UEdStateGraph::GetPropertiesOptions(const FSMPropertySearch& SearchParam) const
{
	TArray<FString> Names = this->GetBlueprintOwner()->GetPropertiesOptions(SearchParam);

	if (!this->IsMainGraph())
	{
		TArray<FString> NewNames;

		for (auto& Name : Names)
		{
			NewNames.Add("../" + Name);
		}

		for (TFieldIterator<FProperty> FIT(this->MachineArchetype->GetClass(), EFieldIteratorFlags::IncludeSuper); FIT; ++FIT)
		{
			FProperty* f = *FIT;

			if (SearchParam.Matches(f))
			{
				NewNames.Add(f->GetName());
			}
		}

		Names = NewNames;
	}

	return Names;
}

void UEdStateGraph::Delete()
{
	this->GetBlueprintOwner()->DeleteGraph(this);
}

void UEdStateGraph::RenameGraph(FName NewName)
{
	this->GetBlueprintOwner()->RenameGraph(this, NewName);
}

void UEdStateGraph::RemoveEvent(UEdEventObject* EventObj)
{
	if (this->EventObjects.Contains(EventObj))
	{
		this->EventObjects.Remove(EventObj);
		EventObj->Events.OnEventRemoved.Broadcast();
	}
}

bool UEdStateGraph::Modify(bool bAlwaysMarkDirty)
{
	Super::Modify(bAlwaysMarkDirty);
	return this->GetBlueprintOwner()->Modify(bAlwaysMarkDirty);	
}

void UEdStateGraph::GetEventEntries(TMap<FName, FEventSetRow>& Entries)
{
	for (auto& EventObj : this->EventObjects)
	{
		Entries.Add(EventObj->GetEventName(), EventObj->GetDescription());
	}
}

FName UEdStateGraph::GetCategoryName() const
{
	if (this->Category == NAME_None)
	{
		return "SubMachines";
	}
	else
	{
		FString CategoryName;
		CategoryName.Append("SubMachines/");
		CategoryName.Append(this->Category.ToString());

		return FName(CategoryName);
	}
}

/*
void UEdStateGraph::CollectExtendibleStates(TSet<FString>& StateNames) const
{
	if (this->IsMainGraph())
	{
		this->GetBlueprintOwner()->GetStateMachineGeneratedClass()->CollectExtendibleStates(StateNames);
	}
	else
	{

		this->MachineArchetype->CollectExtendibleStates(StateNames);
	}
}
*/

FName UEdStateGraph::GetGraphName() const
{
	if (this->IsMainGraph())
	{
		return NAME_None;
	}
	else
	{
		if (this->Accessibility == EStateMachineAccessibility::PRIVATE)
		{
			return this->GetClassPrefix();
		}
		else
		{
			return this->GetFName();
		}
	}
}

bool UEdStateGraph::UpdateOverrideData()
{
	bool bOverrideWasModified = false;

	if (this->GraphType == EStateMachineGraphType::EXTENDED_GRAPH)
	{
		if (this->OverridenMachine.IsNone())
		{
			this->Accessibility = EStateMachineAccessibility::PRIVATE;
			this->MachineArchetypeOverride.Value = nullptr;
		}
		else if (!this->IsMainGraph()) // Sanity Check.
		{
			if (this->GetFName() != this->OverridenMachine)
			{
				this->RenameGraph(this->OverridenMachine);
			}

			if (!IsValid(this->MachineArchetypeOverride.DefaultObject))
			{
				if (auto BPObj = this->GetBlueprintOwner())
				{
					if (auto Class = BPObj->GetStateMachineGeneratedClass()->GetParent())
					{
						auto Data = Class->GetMachineArchetypeData(this->OverridenMachine);

						this->Accessibility = Class->GetSubMachineAccessibility(this->OverridenMachine);
						this->MachineArchetypeOverride.DefaultObject = DuplicateObject(Data->GetArchetype(), this);
						this->MachineArchetypeOverride.Value = DuplicateObject(Data->GetArchetype(), this);
					}
				}
			}
			else
			{
				if (auto BPObj = this->GetBlueprintOwner())
				{
					if (auto Class = BPObj->GetStateMachineGeneratedClass()->GetParent())
					{
						auto Data = Class->GetMachineArchetypeData(this->OverridenMachine);

						bOverrideWasModified = UPropertyManagementLibrary::UpdateObjectInheritanceProperties<UStateMachine>(
							Data->GetArchetype(),
							this->MachineArchetypeOverride.DefaultObject,
							this->MachineArchetypeOverride.Value);
					}
				}
			}
		}
	}
	else
	{
		this->OverridenMachine = NAME_None;
		this->MachineArchetypeOverride.Value = nullptr;
		this->MachineArchetypeOverride.DefaultObject = nullptr;
	}

	return bOverrideWasModified;
}

void UEdStateGraph::VerifyMachineArchetypes(FNodeVerificationContext& Context) const
{
	if (!this->IsMainGraph())
	{
		if (this->GraphType == EStateMachineGraphType::EXTENDED_GRAPH)
		{
			if (auto Overriden = this->GetBlueprintOwner()->GetStateMachineGeneratedClass()->GetParent()->GetMostRecentParentArchetype(this->GetFName()))
			{
				auto SourceMachine = this->GetSourceMachine();
				if (!SourceMachine->IsA(Overriden->GetClass()))
				{
					FString ErrorMessage = FString::Printf(
						TEXT("Overriding Archetype is not a subclass of the parent graph's archetype. Parent Class: %s, Overriding Type: %s"),
						*Overriden->GetClass()->GetName(),
						*SourceMachine->GetClass()->GetName());
					Context.Error(ErrorMessage, this);
				}
			}
			else
			{
				FString ErrorMessage = FString::Printf(
					TEXT("Overriding null parent machine archetype in SubGraph %s (Internal Error to be reported)."), 
					*this->GetName());
				Context.Error(ErrorMessage, this);
			}
		}
	}
}

TArray<FString> UEdStateGraph::GetInheritableStates(EStateNodeType NodeType) const
{
	TSet<FString> Names;

	if (auto BPGC = this->GetBlueprintOwner()->GetStateMachineGeneratedClass()->GetParent())
	{
		if (NodeType == EStateNodeType::EXTENDED_NODE)
		{
			BPGC->CollectExtendibleStates(Names, this->GetGraphName());
		}
		else if (NodeType == EStateNodeType::OVERRIDE_NODE)
		{
			BPGC->CollectOverrideableStates(Names, this->GetGraphName());
		}
	}

	for (auto& States : this->GetStateOptions(this))
	{
		Names.Remove(States);
	}

	return Names.Array();
}

TSubclassOf<UState> UEdStateGraph::GetStateClass() const
{
	if (this->DefaultStateClass)
	{
		return this->DefaultStateClass;
	}
	else
	{
		return this->GetBlueprintOwner()->GetStateClass();
	}
}

void UEdStateGraph::UpdateDefaultStateClass(TSubclassOf<UState> StateClass)
{
	if (!this->DefaultStateClass)
	{
		for (auto& Node : this->Nodes)
		{
			if (auto StateNode = Cast<UEdStateNode>(Node))
			{
				StateNode->UpdateStateArchetype(StateClass);
			}
		}
	}
}

void UEdStateGraph::OnDebugStateChanged(const FStateChangedEventData& Data)
{
	if (auto StateNode = this->GetStateNodeByName(Data.To))
	{
		StateNode->SetDebugObject(Data.ToState);
	}
}

#if WITH_EDITOR

void UEdStateGraph::PostLoad()
{
	Super::PostLoad();
}

void UEdStateGraph::PostEditUndo()
{
	Super::PostEditUndo();
	UEdGraph::NotifyGraphChanged();
	this->Events.OnGraphDataReverted.Broadcast();
}

void UEdStateGraph::PostEditChangeProperty(
	FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	this->Modify();

	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UEdStateGraph, EventSets))
	{
		TSet<TObjectPtr<UDataTable>> FilteredEventSets;

		for (auto& DT : this->EventSets)
		{
			if (DT->GetRowStruct() == FEventSetRow::StaticStruct())
			{
				FilteredEventSets.Add(DT);
			}
		}

		this->EventSets = FilteredEventSets;
	}
	else if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UEdStateGraph, Accessibility))
	{
		if (!StateMachineAccessibility::IsPublic(this->Accessibility))
		{
			this->bIsVariable = false;
		}
	}
	else if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UEdStateGraph, OverridenMachine))
	{
		this->UpdateOverrideData();
	}
	else if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UEdStateGraph, MachineArchetypeOverride))
	{
		// Only do this if the value of this was reset to null.
		if (this->MachineArchetypeOverride.Value == nullptr)
		{
			this->UpdateOverrideData();
		}
	}
	else if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UEdStateGraph, MachineArchetypeOverride))
	{
		if (this->GraphType != EStateMachineGraphType::EXTENDED_GRAPH)
		{
			this->OverridenMachine = NAME_None;
			this->RenameGraph(this->GetBlueprintOwner()->GetNewGraphName());
		}
	}
	else if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UEdStateGraph, DefaultStateClass))
	{
		if (this->DefaultStateClass)
		{
			this->UpdateDefaultStateClass(this->DefaultStateClass);
		}		
	}
}

TArray<FString> UEdStateGraph::GetOverrideableMachines() const
{
	TArray<FString> Names;

	if (!this->IsMainGraph())
	{
		if (auto BPObj = this->GetBlueprintOwner())
		{
			if (auto Class = Cast<UStateMachineBlueprintGeneratedClass>(BPObj->GetStateMachineGeneratedClass()->GetSuperClass()))
			{
				Names.Append(Class->GetChildAccessibleSubMachines());

				// Remove any that have been extended.
				for (auto& SubMachine : BPObj->GetDefinedSubMachines())
				{
					Names.Remove(SubMachine);
				}
			}
		}
	}

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}

void UEdStateGraph::PostLinkerChange()
{
	
}

#endif // WITH_EDITOR

#undef LOCTEXT_NAMESPACE