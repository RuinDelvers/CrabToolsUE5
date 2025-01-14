#include "StateMachine/StateMachineBlueprint.h"
#include "StateMachine/StateMachineInterface.h"
#include "StateMachine/StateMachineBlueprintGeneratedClass.h"
#include "StateMachine/EdGraph/EdStartStateNode.h"
#include "StateMachine/EdGraph/EdStateGraph.h"
#include "StateMachine/EdGraph/EdEventObject.h"
#include "StateMachine/EdGraph/StateMachineSchema.h"
#include "StateMachine/DataStructures.h"
#include "Engine/DataTable.h"

#define LOCTEXT_NAMESPACE "UStateMachineBlueprint"
#include "EdGraph/EdGraph.h"
#include "Kismet2/BlueprintEditorUtils.h"

#define DEFAULT_STATEMACHINE_NAME "NewStateMachine"

UStateMachineBlueprint::UStateMachineBlueprint(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer),
	MainGraph(nullptr),
	DefaultStateClass(UState::StaticClass())
{
	
}

UClass* UStateMachineBlueprint::GetBlueprintClass() const
{
	return UStateMachineBlueprintGeneratedClass::StaticClass();
}

bool UStateMachineBlueprint::SupportsInputEvents() const 
{
	return true;
}

UEdStateGraph* UStateMachineBlueprint::GetMainGraph()
{
	if (this->MainGraph == nullptr)
	{
		this->MainGraph = CastChecked<UEdStateGraph>(FBlueprintEditorUtils::CreateNewGraph(
			this,
			"MainGraph",
			UEdStateGraph::StaticClass(),
			UStateMachineSchema::StaticClass()));

		this->MainGraph->SetGraphType(EStateMachineGraphType::MAIN_GRAPH);
		const UEdGraphSchema* Schema = this->MainGraph->GetSchema();
		Schema->CreateDefaultNodesForGraph(*this->MainGraph);

		this->MainGraph->Initialize(this);
	}

	return this->MainGraph;
}

UEdStateGraph* UStateMachineBlueprint::AddSubGraph()
{
	auto NewGraph = CastChecked<UEdStateGraph>(FBlueprintEditorUtils::CreateNewGraph(
		this,
		this->GetNewGraphName(),
		UEdStateGraph::StaticClass(),
		UStateMachineSchema::StaticClass()));

	NewGraph->SetGraphType(EStateMachineGraphType::SUB_GRAPH);

	const UEdGraphSchema* Schema = NewGraph->GetSchema();
	Schema->CreateDefaultNodesForGraph(*NewGraph);

	this->SubGraphs.Add(NewGraph);

	NewGraph->Initialize(this);

	return NewGraph;
}

void UStateMachineBlueprint::SetObjectBeingDebugged(UObject* Obj)
{
	Super::SetObjectBeingDebugged(Obj);

	if (auto SM = Cast<UStateMachine>(Obj))
	{
		this->MainGraph->SetDebugMachine(SM);

		for (const auto& SubGraph : this->SubGraphs)
		{
			auto Name = SubGraph->GetGraphName().ToString();

			if (auto SubMachine = Cast<UStateMachine>(SM->GetSubMachine(Name)))
			{
				SubGraph->SetDebugMachine(SubMachine);
			}
		}
	}
	else
	{
		this->MainGraph->SetDebugMachine(nullptr);

		for (const auto SubGraph : this->SubGraphs)
		{
			SubGraph->SetDebugMachine(nullptr);
		}
	}
}

FName UStateMachineBlueprint::GetNewGraphName() const
{
	FString DefaultName(DEFAULT_STATEMACHINE_NAME);
	int index = 0;

	bool IsAvailable = false;

	while (!IsAvailable)
	{
		index += 1;

		FString NameCheck(DefaultName);
		NameCheck.AppendInt(index);

		IsAvailable = this->IsGraphNameAvailable(NameCheck);		
	}

	DefaultName.AppendInt(index);

	return FName(DefaultName);
}

bool UStateMachineBlueprint::IsGraphNameAvailable(FString& Name) const
{
	if (Name == this->MainGraph->GetGraphName())
	{
		return false;
	}
	else
	{
		for (auto& SubGraph : this->SubGraphs)
		{
			if (SubGraph->GetName() == Name)
			{
				return false;
			}
		}

		if (auto BPGC = this->GetStateMachineGeneratedClass())
		{
			return !BPGC->IsSubMachineNameInUse(Name);
		}
		else
		{
			return true;
		}
	}
}

bool UStateMachineBlueprint::IsEventNameAvailable(FName Name) const
{
	for (auto& IFace : this->Interfaces)
	{
		if (IFace.LoadSynchronous()->HasEvent(Name))
		{
			return false;
		}
	}

	return true;
}

void UStateMachineBlueprint::Verify(FNodeVerificationContext& Context) const
{
	for (auto& IFace : this->Interfaces)
	{
		auto SMOptions = this->GetMachineOptions();
		auto LoadedIFace = IFace.LoadSynchronous();

		for (auto& SMName : LoadedIFace->GetSubMachines())
		{
			if (!SMOptions.Contains(SMName.ToString()))
			{
				FString ErrorMessage = FString::Printf(
					TEXT("SubMachine %s not implemented for interface %s"),
					*SMName.ToString(),
					*IFace->GetName());

				Context.Error(ErrorMessage, this);
			}
		}

		this->MainGraph->Verify(Context, IFace.LoadSynchronous());
	}
}

bool UStateMachineBlueprint::IsMainGraph(const UEdStateGraph* Graph) const
{
	return this->MainGraph == Graph;
}

void UStateMachineBlueprint::RenameGraph(UEdStateGraph* Graph, FName Name)
{
	auto Renamed = this->SubGraphs.Find(Graph);

	if (Renamed >= 0)
	{
		const FScopedTransaction Transaction(LOCTEXT("RenameGraph", "Rename Graph"));
		this->Modify();
		Graph->Modify();

		FName OldName = Graph->GetFName();
		Graph->Rename(*Name.ToString(), this);
		Graph->Events.OnNameChanged.Broadcast(OldName, Name);
	}
}

bool UStateMachineBlueprint::Modify(bool bAlwaysMarkDirty)
{
	//FBlueprintEditorUtils::MarkBlueprintAsModified(this);
	return Super::Modify(bAlwaysMarkDirty);	
}

void UStateMachineBlueprint::SelectGraph(UEdStateGraph* Graph)
{
	if (Graph == this->MainGraph)
	{
		this->Events.OnGraphSelected.Broadcast(Graph);
	}
	else if (this->SubGraphs.Contains(Graph))
	{
		this->Events.OnGraphSelected.Broadcast(Graph);
	}
}

TArray<FString> UStateMachineBlueprint::GetDefinedSubMachines() const
{
	TArray<FString> Names;

	for (auto& SubM : this->SubGraphs)
	{
		Names.Add(SubM->GetGraphName().ToString());
	}

	return Names;
}

void UStateMachineBlueprint::InspectObject(UObject* Obj)
{
	this->Events.OnObjectInspected.Broadcast(Obj);
}

TArray<FString> UStateMachineBlueprint::GetMachineOptions() const
{
	TSet<FString> Names;

	for (auto& Graph : this->SubGraphs)
	{
		Names.Add(Graph->GetGraphName().ToString());
	}

	if (auto BPGC = Cast<UStateMachineBlueprintGeneratedClass>(this->GeneratedClass))
	{
		Names.Append(BPGC->GetChildAccessibleSubMachines());
	}
	
	return Names.Array();
}


TSet<FName> UStateMachineBlueprint::GetEventSet() const
{
	TSet<FName> EventNames;

	for (auto& Ev : this->MainGraph->GetEventList())
	{
		EventNames.Add(Ev->GetEventName());
	}

	for (auto& SubGraph : this->SubGraphs)
	{
		for (auto& Ev : SubGraph->GetEventList())
		{
			EventNames.Add(Ev->GetEventName());
		}
	}

	return EventNames;
}

void UStateMachineBlueprint::AppendInterfaceEvents(TArray<FString>& Names) const
{
	for (auto& IFacePtr : this->Interfaces)
	{
		if (auto IFace = IFacePtr.LoadSynchronous())
		{
			for (auto& EName : IFace->GetEvents())
			{
				Names.AddUnique(EName.ToString());
			}
		}
	}
}

TArray<FString> UStateMachineBlueprint::GetPropertiesOptions(const FSMPropertySearch& SearchParam) const
{
	TArray<FString> Names;

	for (TFieldIterator<FProperty> FIT(this->GeneratedClass, EFieldIteratorFlags::IncludeSuper); FIT; ++FIT)
	{
		FProperty* f = *FIT;

		if (SearchParam.Matches(f))
		{
			Names.Add(f->GetName());
		}
	}

	for (auto& SubMachine : this->SubGraphs)
	{
		for (TFieldIterator<FProperty> FIT(SubMachine->GetMachineArchetype()->GetClass(), EFieldIteratorFlags::IncludeSuper); FIT; ++FIT)
		{
			FProperty* f = *FIT;

			if (SearchParam.Matches(f))
			{
				FString Formatted = FString::Printf(
					TEXT("%s/%s"), 
					*SubMachine->GetGraphName().ToString(), 
					*f->GetName());
				Names.Add(Formatted);
			}
		}
	}

	return Names;
}

void UStateMachineBlueprint::DeleteGraph(UEdStateGraph* Graph)
{
	if (this->SubGraphs.Contains(Graph))
	{
		const FScopedTransaction Transaction(LOCTEXT("DeleteStateMachine", "Delete State Machine"));
		this->Modify();
		FBlueprintEditorUtils::MarkBlueprintAsModified(this);
		this->SubGraphs.Remove(Graph);
		Graph->Events.OnGraphDeleted.Broadcast();
	}
}

void UStateMachineBlueprint::ClearDelegates()
{
	this->Events.OnGraphSelected.Clear();
	this->Events.OnObjectInspected.Clear();
}

void UStateMachineBlueprint::GetEventEntries(TMap<FName, FEventSetRow>& Entries)
{
	this->MainGraph->GetEventEntries(Entries);

	for (auto Graph : this->SubGraphs)
	{
		Graph->GetEventEntries(Entries);
	}
}

void UStateMachineBlueprint::AddEventsToDataTable(UDataTable* EventSet, bool bClearEvents)
{
	if (EventSet->RowStruct == FEventSetRow::StaticStruct())
	{
		const FScopedTransaction Transaction(LOCTEXT("EditEventSet", "Add To Event Set"));
		EventSet->Modify();

		if (bClearEvents)
		{
			EventSet->EmptyTable();
		}

		TMap<FName, FEventSetRow> EntryMap;
		this->GetEventEntries(EntryMap);

		for (auto Entry : EntryMap)
		{
			if (!EventSet->FindRow<FEventSetRow>(Entry.Key, "", false))
			{
				EventSet->AddRow(Entry.Key, Entry.Value);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Data Table has wrong type of row."));
	}
}

bool UStateMachineBlueprint::HasEvent(FName EName) const
{
	bool bHasEvent = false;

	for (auto& Interface : this->Interfaces)
	{
		if (Interface->HasEvent(EName))
		{
			bHasEvent = true;
		}
	}

	return bHasEvent;
}

UStateMachineBlueprintGeneratedClass* UStateMachineBlueprint::GetStateMachineGeneratedClass() const
{
	return Cast<UStateMachineBlueprintGeneratedClass>(this->GeneratedClass);
}

void UStateMachineBlueprint::UpdateDefaultStateClass()
{
	this->MainGraph->UpdateDefaultStateClass(this->DefaultStateClass);

	for (auto& Graph : this->SubGraphs)
	{
		Graph->UpdateDefaultStateClass(this->DefaultStateClass);
	}
}

#if WITH_EDITOR

void UStateMachineBlueprint::PostEditChangeProperty(
	FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	this->Modify();

	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UStateMachineBlueprint, DefaultStateClass))
	{
		this->UpdateDefaultStateClass();
	}
}

#endif // WITH_EDITOR

#undef LOCTEXT_NAMESPACE