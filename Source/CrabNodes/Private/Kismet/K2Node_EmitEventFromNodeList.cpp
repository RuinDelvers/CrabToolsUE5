// Copyright Epic Games, Inc. All Rights Reserved.

#include "Kismet/K2Node_EmitEventFromNodeList.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "Containers/EnumAsByte.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraphSchema_K2.h"
#include "Engine/MemberReference.h"
#include "Internationalization/Internationalization.h"
#include "K2Node_CallFunction.h"
#include "Kismet2/CompilerResultsLog.h"
#include "KismetCompiler.h"
#include "Math/Color.h"
#include "Misc/AssertionMacros.h"
#include "Styling/AppStyle.h"
#include "Templates/Casts.h"
#include "UObject/Class.h"
#include "UObject/NameTypes.h"
#include "StateMachine/StateMachine.h"

class UBlueprint;

#define LOCTEXT_NAMESPACE "K2Node_EmitEventFromNodeList"

namespace EmitEventFromNodeListHelper
{
	const FName EventPinName = "Event";
	const FName SelfPinName = "self";
}

UK2Node_EmitEventFromNodeList::UK2Node_EmitEventFromNodeList(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeTooltip = LOCTEXT("NodeTooltip", "Emits an event from the emitted list for this node.");
}

void UK2Node_EmitEventFromNodeList::AllocateDefaultPins()
{
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	// Add execution pins
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	UEdGraphPin* RowFoundPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);

	// Add State Machine Pin
	UEdGraphPin* SelfPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UEdGraphSchema_K2::PSC_Self, UStateNode::StaticClass(), EmitEventFromNodeListHelper::SelfPinName);
	SetPinToolTip(*SelfPin, LOCTEXT("SelfPinDescription", "Self"));
	SelfPin->bHidden = true;

	// Row Name pin
	UEdGraphPin* EventPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Name, EmitEventFromNodeListHelper::EventPinName);
	SetPinToolTip(*EventPin, LOCTEXT("EventPinDescription", "The name of the row to retrieve from the Interface"));

	Super::AllocateDefaultPins();
}

void UK2Node_EmitEventFromNodeList::SetPinToolTip(UEdGraphPin& MutatablePin, const FText& PinDescription) const
{
	MutatablePin.PinToolTip = UEdGraphSchema_K2::TypeToText(MutatablePin.PinType).ToString();

	UEdGraphSchema_K2 const* const K2Schema = Cast<const UEdGraphSchema_K2>(GetSchema());
	if (K2Schema != nullptr)
	{
		MutatablePin.PinToolTip += TEXT(" ");
		MutatablePin.PinToolTip += K2Schema->GetPinDisplayName(&MutatablePin).ToString();
	}

	MutatablePin.PinToolTip += FString(TEXT("\n")) + PinDescription.ToString();
}

void UK2Node_EmitEventFromNodeList::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	// actions get registered under specific object-keys; the idea is that 
	// actions might have to be updated (or deleted) if their object-key is  
	// mutated (or removed)... here we use the node's class (so if the node 
	// type disappears, then the action should go with it)
	UClass* ActionKey = GetClass();
	// to keep from needlessly instantiating a UBlueprintNodeSpawner, first   
	// check to make sure that the registrar is looking for actions of this type
	// (could be regenerating actions for a specific asset, and therefore the 
	// registrar would only accept actions corresponding to that asset)
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

FText UK2Node_EmitEventFromNodeList::GetMenuCategory() const
{
	return LOCTEXT("Category", "State Machine");
}

bool UK2Node_EmitEventFromNodeList::IsConnectionDisallowed(const UEdGraphPin* MyPin, const UEdGraphPin* OtherPin, FString& OutReason) const
{
	return false;
}

void UK2Node_EmitEventFromNodeList::PinDefaultValueChanged(UEdGraphPin* ChangedPin) 
{
	this->RefreshEventOptions();
}

FText UK2Node_EmitEventFromNodeList::GetTooltipText() const
{
	return NodeTooltip;
}

UEdGraphPin* UK2Node_EmitEventFromNodeList::GetThenPin()const
{
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	UEdGraphPin* Pin = FindPinChecked(UEdGraphSchema_K2::PN_Then);
	check(Pin->Direction == EGPD_Output);
	return Pin;
}

UEdGraphPin* UK2Node_EmitEventFromNodeList::GetEventPin() const
{
	UEdGraphPin* Pin = FindPinChecked(EmitEventFromNodeListHelper::EventPinName);
	check(Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UK2Node_EmitEventFromNodeList::GetSelfPin() const
{
	UEdGraphPin* Pin = FindPinChecked(EmitEventFromNodeListHelper::SelfPinName);
	check(Pin->Direction == EGPD_Input);
	return Pin;
}

FText UK2Node_EmitEventFromNodeList::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (TitleType == ENodeTitleType::MenuTitle)
	{
		return LOCTEXT("ListViewTitle", "Emit Event");
	}
	else
	{
		return LOCTEXT("Interface_Title", "Emit Event");
	}
}

void UK2Node_EmitEventFromNodeList::ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
    Super::ExpandNode(CompilerContext, SourceGraph);

	// FUNCTION NODE
	const FName FunctionName = GET_FUNCTION_NAME_CHECKED(UStateNode, EmitEvent);
	UK2Node_CallFunction* EmitEventFromNodeListFunction = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	EmitEventFromNodeListFunction->FunctionReference.SetExternalMember(FunctionName, UStateNode::StaticClass());
	EmitEventFromNodeListFunction->AllocateDefaultPins();
    CompilerContext.MovePinLinksToIntermediate(*GetExecPin(), *(EmitEventFromNodeListFunction->GetExecPin()));

	// Connect the input of our EmitEventFromNodeList to the Input of our Function pin
    UEdGraphPin* SelfInPin = EmitEventFromNodeListFunction->FindPinChecked(TEXT("self"));
	CompilerContext.MovePinLinksToIntermediate(*GetSelfPin(), *SelfInPin);

	UEdGraphPin* EventInPin = EmitEventFromNodeListFunction->FindPinChecked(TEXT("InEvent"));
	CompilerContext.MovePinLinksToIntermediate(*GetEventPin(), *EventInPin);

	// Get some pins to work with
    UEdGraphPin* FunctionThenPin = EmitEventFromNodeListFunction->GetThenPin();
                
    // Hook up outputs
    CompilerContext.MovePinLinksToIntermediate(*GetThenPin(), *FunctionThenPin);

	BreakAllNodeLinks();
}

FSlateIcon UK2Node_EmitEventFromNodeList::GetIconAndTint(FLinearColor& OutColor) const
{
	OutColor = GetNodeTitleColor();
	static FSlateIcon Icon(FAppStyle::GetAppStyleSetName(), "Kismet.AllClasses.FunctionIcon");
	return Icon;
}

void UK2Node_EmitEventFromNodeList::NotifyPinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::NotifyPinConnectionListChanged(Pin);

	RefreshEventOptions();
}

bool UK2Node_EmitEventFromNodeList::IsActionFilteredOut(FBlueprintActionFilter const& Filter)
{
	bool bAllBlueprintSupported = true;

	for (const auto& BP : Filter.Context.Blueprints)
	{
		if (BP)
		{
			if (BP->ParentClass && !BP->ParentClass->IsChildOf<UStateNode>())
			{
				bAllBlueprintSupported = false;
				break;
			}
		}
	}

	return !bAllBlueprintSupported;
}

void UK2Node_EmitEventFromNodeList::RefreshEventOptions()
{
	// When the Interface pin gets a new value assigned, we need to update the Slate UI so that SGraphNodeCallParameterCollectionFunction will update the ParameterName drop down
	UEdGraph* Graph = GetGraph();
	Graph->NotifyNodeChanged(this);

	auto EventSet = this->GetEventSet();
	this->OnEventSetChanged.Broadcast(EventSet);
}

void UK2Node_EmitEventFromNodeList::PostReconstructNode()
{
	Super::PostReconstructNode();
}

void UK2Node_EmitEventFromNodeList::EarlyValidation(class FCompilerResultsLog& MessageLog) const
{
	Super::EarlyValidation(MessageLog);

	//const UEdGraphPin* InterfacePin = GetInterfacePin();
	const UEdGraphPin* EventPin = GetEventPin();

	if (!EventPin)
	{
		MessageLog.Error(*LOCTEXT("MissingPins", "Missing pins in @@").ToString(), this);
		return;
	}
}

TSet<FName> UK2Node_EmitEventFromNodeList::GetEventSet() const
{
	TSet<FName> Emitted;

	if (auto Node = Cast<UStateNode>(this->GetSelfPin()->DefaultObject))
	{
		Node->GetEmittedEvents(Emitted);
	}

	if (auto BPGC = this->GetBlueprint()->GeneratedClass.Get())
	{
		if (auto Node = Cast<UStateNode>(BPGC->GetDefaultObject(false)))
		{
			Node->GetEmittedEvents(Emitted);
		}
	}

	return Emitted;
}

#undef LOCTEXT_NAMESPACE
