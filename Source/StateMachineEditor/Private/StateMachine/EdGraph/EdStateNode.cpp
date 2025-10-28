#include "StateMachine/EdGraph/EdStateNode.h"
#include "StateMachine/EdGraph/EdStateGraph.h"
#include "StateMachine/EdGraph/EdTransition.h"
#include "StateMachine/StateMachineBlueprint.h"
#include "StateMachine/StateMachineBlueprintGeneratedClass.h"
#include "StateMachine/Utils.h"
#include "Utils/PropertyManagement.h"

#include "StateMachine/ArrayNode.h"

#define LOCTEXT_NAMESPACE "EdStateNode"

UEdStateNode::UEdStateNode() {
	this->bCanRenameNode = true;

	this->StateClass = CreateDefaultSubobject<UState>(TEXT("DefaultStateClass"));
}

UEdStateNode::~UEdStateNode()
{

}

FName UEdStateNode::GetStateName() const
{ 
	if (this->Accessibility == EStateMachineAccessibility::PRIVATE)
	{
		FString Total;

		Total.Append(this->GetStateGraph()->GetClassPrefix().ToString());
		Total.Append("::");
		Total.Append(this->StateName.ToString());

		return FName(Total);
	}
	else
	{
		return this->StateName;
	}
}

TSet<FName> UEdStateNode::GetNotifies() const
{
	TSet<FName> Notifies;

	for (const auto& Node : this->Nodes)
	{
		Node->GetNotifies(Notifies);
	}

	return Notifies;
}

FName UEdStateNode::SetStateName(FName NewName)
{
	FScopedTransaction RenameTransaction(LOCTEXT("StateNodeRename", "Rename State Node"));
	this->Modify();
	this->OnModify();
	FName OldName = this->StateName;
	this->StateName = NewName;	
	this->Events.OnNameChanged.Broadcast(OldName, NewName);

	return NewName;
}

void UEdStateNode::RenameNode(FName Name)
{
	this->GetStateGraph()->RenameNode(this, Name);
}

FStateArchetypeData UEdStateNode::GetBaseCompilationData(UObject* Outer)
{
	FStateArchetypeData Data;

	// This should always be copied from archetypes if this node extends/overrides.
	Data.Access = this->Accessibility;

	switch (this->NodeType)
	{
		case EStateNodeType::INLINE_NODE:

			if (!IsValid(this->StateClass))
			{
				this->StateClass = NewObject<UState>(this);
			}

			Data.SetArchetype(DuplicateObject(this->StateClass, Outer));
			
			break;

		case EStateNodeType::EXTENDED_NODE:
			Data.SetArchetype(NewObject<UState>(Outer));
			Data.bIsExtension = true;
			break;

		case EStateNodeType::OVERRIDE_EXTENDED_NODE: 
			Data.SetArchetype(DuplicateObject(this->StateClassOverride.Value, Outer));
			Data.bIsExtension = true;
			Data.bIsOverride = true;
			break;

		case EStateNodeType::OVERRIDE_NODE:
			Data.SetArchetype(DuplicateObject(this->StateClassOverride.Value, Outer));
			Data.bIsOverride = true;
			break;
	}

	return Data;
}

FStateArchetypeData UEdStateNode::CompileState(FNodeVerificationContext& Context, UObject* Outer)
{
	if (this->UpdateStateArchetypeOverride())
	{
		FString ErrorMessage = FString::Printf(
			TEXT("Subgraph %s::%s::%s overriden state is out of sync with its parent and has been modified."),
			*this->GetStateGraph()->GetBlueprintOwner()->GetName(),
			*this->GetStateGraph()->GetGraphName().ToString(),
			*this->GetNodeName().ToString());

		Context.Warning(ErrorMessage, this);
	}

	for (auto& Node : this->Nodes)
	{
		Node->Verify(Context);
	}

	FStateArchetypeData Data = this->GetBaseCompilationData(Outer);

	if (this->Nodes.Num() == 1)
	{
		Data.GetArchetype()->AppendNodeCopy(this->Nodes[0]);
	}
	else
	{
		auto ArrayNode = NewObject<UArrayNode>(Data.GetArchetype());

		for (auto& Node : this->Nodes)
		{
			ArrayNode->AddNode(DuplicateObject(
				Node, 
				ArrayNode, 
				Naming::GenerateStateNodeName(Node, this->GetStateName())));
		}

		Data.GetArchetype()->AppendNode(ArrayNode);
	}

	return Data;
}

TArray<FString> UEdStateNode::GetEventOptions() const
{
	TSet<FName> EventsSet;

	for (auto& Node : this->Nodes)
	{
		if (IsValid(Node))
		{
			Node->GetEmittedEvents(EventsSet);
		}
	}

	if (this->NodeType == EStateNodeType::EXTENDED_NODE)
	{
		if (auto Parent = this->GetStateGraph()->GetBlueprintOwner()->GetStateMachineGeneratedClass()->GetParent())
		{
			Parent->GetStateEmittedEvents(this->GetStateGraph()->GetGraphName(), this->GetStateName(), EventsSet);
		}
	}

	TArray<FString> EventArray;
	for (auto& Event : EventsSet) { EventArray.Add(Event.ToString()); }
	for (auto& Event : this->GetStateGraph()->GetEventOptions()) { EventArray.Add(Event); }
	
	EventArray.Sort([&](const FString& A, const FString& B) { return A < B; });

	return EventArray;
}

void UEdStateNode::GetLocalEventOptions(TArray<FString>& Names) const
{
	TSet<FName> PerNodeNames;

	for (const auto& Node : this->Nodes)
	{
		Node->GetEmittedEvents(PerNodeNames);
	}

	for (const auto& Name : PerNodeNames)
	{
		Names.Add(Name.ToString());
	}
}

void UEdStateNode::Delete()
{
	this->Modify();

	const UEdGraphSchema* Schema = this->GetSchema();
	if (Schema != nullptr)
	{
		Schema->BreakNodeLinks(*this);
	}

	this->DestroyNode();
}

bool UEdStateNode::Modify(bool bAlwaysMarkDirty)
{
	Super::Modify(bAlwaysMarkDirty);
	return this->GetGraph()->Modify(bAlwaysMarkDirty);
}

bool UEdStateNode::HasEvent(FName InEvent)
{
	if (this->NodeType == EStateNodeType::EXTENDED_NODE)
	{
		if (auto Parent = this->GetStateGraph()->GetBlueprintOwner()->GetStateMachineGeneratedClass()->GetParent())
		{
			TSet<FName> ParentEvents;

			Parent->GetStateEmittedEvents(this->GetStateGraph()->GetGraphName(), this->GetStateName(), ParentEvents);

			if (ParentEvents.Contains(InEvent))
			{
				return true;
			}
		}
	}

	this->UpdateEmittedEvents();
	return this->NodeEmittedEvents.Contains(InEvent) || this->GetStateGraph()->HasEvent(InEvent);
}

bool UEdStateNode::HasLocalEvent(FName InEvent)
{
	this->UpdateEmittedEvents();
	return this->NodeEmittedEvents.Contains(InEvent);
}

void UEdStateNode::UpdateEmittedEvents()
{
	this->NodeEmittedEvents.Empty();

	for (auto& Node : this->Nodes)
	{
		if (Node)
		{
			TSet<FName> PartEvents;

			Node->GetEmittedEvents(PartEvents);

			this->NodeEmittedEvents.Append(PartEvents);
		}
	}
}

TArray<FString> UEdStateNode::GetInheritableStates() const
{
	TArray<FString> Names = this->GetStateGraph()->GetInheritableStates(this->NodeType);

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}

bool UEdStateNode::IsActive() const
{
	if (this->DebugStateObject)
	{
		return this->DebugStateObject->IsActive();
	}
	else
	{
		return false;
	}
}

bool UEdStateNode::DoesReferenceMachine(FName MachineName) const
{
	for (const auto& Node : this->Nodes)
	{
		if (Node && Node->DoesReferenceMachine(MachineName))
		{
			return true;
		}
	}

	return false;
}

bool UEdStateNode::UpdateStateArchetypeOverride()
{
	bool bOverrideWasModified = false;

	if (this->NodeType == EStateNodeType::EXTENDED_NODE 
		|| this->NodeType == EStateNodeType::OVERRIDE_EXTENDED_NODE)
	{
		if (this->StateExtension.IsNone())
		{
			this->Accessibility = EStateMachineAccessibility::PRIVATE;
			this->StateClassOverride.Value = nullptr;
		}
		else
		{
			if (this->GetFName() != this->StateExtension)
			{
				this->RenameNode(this->StateExtension);
			}

			if (!IsValid(this->StateClassOverride.DefaultObject))
			{
				if (auto BPObj = this->GetStateGraph()->GetBlueprintOwner())
				{
					if (auto Class = BPObj->GetStateMachineGeneratedClass()->GetParent())
					{
						auto Data = Class->GetStateArchetypeData(this->StateExtension, this->GetStateGraph()->GetFName());
						check(Data);
						auto DuplicatedState = DuplicateObject(Data->GetArchetype(), this);
						this->Accessibility = Data->Access;
						this->StateClassOverride.DefaultObject = DuplicatedState;
						this->StateClassOverride.Value = DuplicateObject(DuplicatedState, this);
					}
				}
			}
			else
			{
				if (auto BPObj = this->GetStateGraph()->GetBlueprintOwner())
				{
					if (auto Class = BPObj->GetStateMachineGeneratedClass()->GetParent())
					{
						auto Data = Class->GetStateArchetypeData(this->StateExtension, this->GetStateGraph()->GetFName());
						check(Data);
						
						bOverrideWasModified = UPropertyManagementLibrary::UpdateObjectInheritanceProperties<UState>(
							Data->GetArchetype(),
							this->StateClassOverride.DefaultObject,
							this->StateClassOverride.Value);
					}
				}
			}
		}
	}
	else
	{
		this->StateExtension = NAME_None;
		this->StateClassOverride.Value = nullptr;
		this->StateClassOverride.DefaultObject = nullptr;
	}

	return bOverrideWasModified;
}

void UEdStateNode::UpdateStateArchetype(TSubclassOf<UState> NewStateClass)
{
	if (this->NodeType == EStateNodeType::INLINE_NODE)
	{
		if (!this->StateClass->IsA(NewStateClass))
		{
			this->StateClass = NewObject<UState>(this, NewStateClass);
		}
	}
}

void UEdStateNode::SetDebugObject(UState* State)
{
	if (this->DebugStateObject != State)
	{
		if (IsValid(State))
		{
			State->OnEventReceived.AddDynamic(this, &UEdStateNode::ReceiveEvent);
			State->OnEventWithDataReceived.AddDynamic(this, &UEdStateNode::ReceiveEventWithData);
		}

		if (this->DebugStateObject)
		{
			this->DebugStateObject->OnEventReceived.RemoveAll(this);
			this->DebugStateObject->OnEventWithDataReceived.RemoveAll(this);
		}

		this->DebugStateObject = State;

		this->FilterDebugData();
	}
}

void UEdStateNode::ReceiveEvent(FName InEvent)
{
	FStateNodeEventDebugData Data;

	Data.EventName = InEvent;
	Data.TimeReceived = this->DebugStateObject->GetMachine()->GetWorld()->GetTimeSeconds();

	this->EventDebugData.Add(Data);
}

void UEdStateNode::ReceiveEventWithData(FName InEvent, UObject* Data)
{
	FStateNodeEventDebugData DebugData;

	DebugData.EventName = InEvent;
	DebugData.bHadData = true;
	DebugData.Data = Data;
	DebugData.TimeReceived = this->DebugStateObject->GetMachine()->GetWorld()->GetTimeSeconds();

	this->EventDebugData.Add(DebugData);
}

void UEdStateNode::FilterDebugData()
{
	if (this->DebugStateObject)
	{
		float Lifetime = this->GetStateGraph()->GetBlueprintOwner()->GetEventDebugDataLifetime();
		float CurrentTime = this->DebugStateObject->GetMachine()->GetWorld()->GetTimeSeconds();

		this->EventDebugData = this->EventDebugData.FilterByPredicate(
			[Lifetime, CurrentTime](const FStateNodeEventDebugData& Data)
			{
				return CurrentTime - Data.TimeReceived <= Lifetime ;
			});
	}
	else
	{
		this->EventDebugData.Empty();
	}
}

FString UEdStateNode::GetDebugDataString()
{
	FString DataString;

	this->FilterDebugData();

	if (this->DebugStateObject)
	{
		float CurrentTime = this->DebugStateObject->GetMachine()->GetWorld()->GetTimeSeconds();

		if (this->IsActive())
		{
			float ActiveTime = this->DebugStateObject->GetMachine()->GetActiveTime();
			DataString.Append(FString::Printf(TEXT("Active Time: %f"), CurrentTime - ActiveTime));
		}
		
		TArray<FString> Joins;

		for (const auto& Data : this->EventDebugData)
		{
			FString Formatted = FString::Printf(
				TEXT("Event received %f ago. \n- %s"),				
				CurrentTime - Data.TimeReceived,
				*Data.EventName.ToString());
			
			Joins.Add(Formatted);
		}

		if (Joins.Num() > 0)
		{
			if (DataString.Len() > 0)
			{
				DataString.Append("\n");
			}
			DataString.Append(FString::Join(Joins, TEXT("\n")));
		}		
	}

	return DataString;
}

#if WITH_EDITOR

void UEdStateNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	this->Modify();

	this->UpdateEmittedEvents();

	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UEdStateNode, NodeType))
	{
		if (this->NodeType == EStateNodeType::INLINE_NODE)
		{
			this->StateExtension = NAME_None;
			this->StateClassOverride.DefaultObject = nullptr;
			this->StateClassOverride.Value = nullptr;
		}
		else
		{
			if (!this->StateExtension.IsNone())
			{
				this->RenameNode(this->StateExtension);
			}
			else
			{
				this->RenameNode(this->GetStateGraph()->GetNewStateName());
			}

			if (this->NodeType == EStateNodeType::EXTENDED_NODE)
			{
				this->StateClass = NewObject<UState>(this);
			}
			else
			{
				this->UpdateStateArchetypeOverride();
			}
		}
	}
	else if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UEdStateNode, StateExtension))
	{
		if (!this->StateExtension.IsNone())
		{
			this->RenameNode(this->StateExtension);
		}
	}

	if (!IsValid(this->StateClass))
	{
		this->StateClass = NewObject<UState>(this);
	}
}

void UEdStateNode::PostEditUndo()
{
	Super::PostEditUndo();

	this->Events.OnNameChanged.Broadcast(this->GetNodeName(), this->GetNodeName());
}

void UEdStateNode::PostLinkerChange()
{
	this->UpdateEmittedEvents();
}
#endif

#undef LOCTEXT_NAMESPACE