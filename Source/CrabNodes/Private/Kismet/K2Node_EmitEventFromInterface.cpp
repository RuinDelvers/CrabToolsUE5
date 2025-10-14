// Copyright Epic Games, Inc. All Rights Reserved.

#include "Kismet/K2Node_EmitEventFromInterface.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraphSchema_K2.h"
#include "Internationalization/Internationalization.h"
#include "K2Node_CallFunction.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/CompilerResultsLog.h"
#include "KismetCompiler.h"
#include "Misc/AssertionMacros.h"
#include "Templates/Casts.h"
#include "UObject/Class.h"
#include "UObject/NameTypes.h"
#include "UObject/ObjectPtr.h"
#include "StateMachine/StateMachineInterface.h"
#include "StateMachine/HelperLibrary.h"

class UBlueprint;

#define LOCTEXT_NAMESPACE "K2Node_EmitEventFromInterface"

namespace EmitEventFromInterfaceHelper
{
	const FName StateMachinePinName = "StateMachine";
	const FName InterfacePinName = "Interface";
	const FName EventPinName = "Event";
}

UK2Node_EmitEventFromInterface::UK2Node_EmitEventFromInterface(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeTooltip = LOCTEXT("NodeTooltip", "Emits an event to a StateMachine from a StateMachineInterface.");
}

void UK2Node_EmitEventFromInterface::AllocateDefaultPins()
{
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	// Add execution pins
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	UEdGraphPin* RowFoundPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);

	// Add State Machine Pin
	UEdGraphPin* StateMachinePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UObject::StaticClass(), EmitEventFromInterfaceHelper::StateMachinePinName);
	SetPinToolTip(*StateMachinePin, LOCTEXT("StateMachinePinDescription", "The EventListener to want to send an event to"));

	// Row Name pin
	// = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Name, EmitEventFromInterfaceHelper::EventPinName);
	UEdGraphPin* EventPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Struct, FEventSlot::StaticStruct(), EmitEventFromInterfaceHelper::EventPinName);
	SetPinToolTip(*EventPin, LOCTEXT("EventPinDescription", "The name of the row to retrieve from the Interface"));

	Super::AllocateDefaultPins();
}

void UK2Node_EmitEventFromInterface::SetPinToolTip(UEdGraphPin& MutatablePin, const FText& PinDescription) const
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

void UK2Node_EmitEventFromInterface::RefreshEventOptions()
{

	// When the Interface pin gets a new value assigned, we need to update the Slate UI so that SGraphNodeCallParameterCollectionFunction will update the ParameterName drop down
	UEdGraph* Graph = GetGraph();
	Graph->NotifyNodeChanged(this);
	//this->OnEventSetChanged.Broadcast({});
}

void UK2Node_EmitEventFromInterface::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
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

FText UK2Node_EmitEventFromInterface::GetMenuCategory() const
{
	return LOCTEXT("Category", "State Machine");
}

void UK2Node_EmitEventFromInterface::PinDefaultValueChanged(UEdGraphPin* ChangedPin) 
{
	if (ChangedPin && ChangedPin->PinName == EmitEventFromInterfaceHelper::InterfacePinName)
	{
		UEdGraphPin* EventPin = GetEventPin();
		UStateMachineInterface* Interface = Cast<UStateMachineInterface>(ChangedPin->DefaultObject);

		if (EventPin)
		{
			RefreshEventOptions();
		}
	}
}

FText UK2Node_EmitEventFromInterface::GetTooltipText() const
{
	return NodeTooltip;
}

UEdGraphPin* UK2Node_EmitEventFromInterface::GetThenPin()const
{
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	UEdGraphPin* Pin = FindPinChecked(UEdGraphSchema_K2::PN_Then);
	check(Pin->Direction == EGPD_Output);
	return Pin;
}

UEdGraphPin* UK2Node_EmitEventFromInterface::GetEventPin() const
{
	UEdGraphPin* Pin = FindPinChecked(EmitEventFromInterfaceHelper::EventPinName);
	check(Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UK2Node_EmitEventFromInterface::GetStateMachinePin() const
{
	UEdGraphPin* Pin = FindPinChecked(EmitEventFromInterfaceHelper::StateMachinePinName);
	check(Pin->Direction == EGPD_Input);
	return Pin;
}

FText UK2Node_EmitEventFromInterface::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (TitleType == ENodeTitleType::MenuTitle)
	{
		return LOCTEXT("ListViewTitle", "Emit Event From Interface");
	}
	else
	{
		return LOCTEXT("Interface_Title", "Emit Event From Interface");	
	}
}

void UK2Node_EmitEventFromInterface::ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
    Super::ExpandNode(CompilerContext, SourceGraph);
    
	this->RefreshDefaultValue();

	UEdGraphPin* OriginalStateMachineInPin = GetStateMachinePin();

	if (this->Event.IsNone())
	{
		CompilerContext.MessageLog.Error(*LOCTEXT("InvalidEventError", "Input event is None.").ToString(), this);
		return;
	}

	// FUNCTION NODE
	const FName FunctionName = GET_FUNCTION_NAME_CHECKED(UStateMachineHelperLibrary, EmitEventSlot);
	UK2Node_CallFunction* EmitEventFromInterfaceFunction = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	EmitEventFromInterfaceFunction->FunctionReference.SetExternalMember(FunctionName, UStateMachineHelperLibrary::StaticClass());
	EmitEventFromInterfaceFunction->AllocateDefaultPins();
    CompilerContext.MovePinLinksToIntermediate(*GetExecPin(), *(EmitEventFromInterfaceFunction->GetExecPin()));

	// Connect the input of our EmitEventFromInterface to the Input of our Function pin
    UEdGraphPin* StateMachineInPin = EmitEventFromInterfaceFunction->FindPinChecked(TEXT("Obj"));

	if(OriginalStateMachineInPin->LinkedTo.Num() > 0)
	{
		// Copy the connection
		CompilerContext.MovePinLinksToIntermediate(*OriginalStateMachineInPin, *StateMachineInPin);
	}
	else
	{
		CompilerContext.MessageLog.Error(*LOCTEXT("NoStateMachine_Error", "EmitEventFromInterface must have a StateMachine specified.").ToString(), this);
		// we break exec links so this is the only error we get
		BreakAllNodeLinks();
		return;
	}

	UEdGraphPin* EventInPin = EmitEventFromInterfaceFunction->FindPinChecked(TEXT("EName"));
	EventInPin->DefaultValue = this->Event.Name().ToString();
	CompilerContext.MovePinLinksToIntermediate(*GetEventPin(), *EventInPin);

	// Get some pins to work with
    UEdGraphPin* FunctionThenPin = EmitEventFromInterfaceFunction->GetThenPin();
                
    // Hook up outputs
    CompilerContext.MovePinLinksToIntermediate(*GetThenPin(), *FunctionThenPin);

	BreakAllNodeLinks();
}

FSlateIcon UK2Node_EmitEventFromInterface::GetIconAndTint(FLinearColor& OutColor) const
{
	OutColor = GetNodeTitleColor();
	static FSlateIcon Icon(FAppStyle::GetAppStyleSetName(), "Kismet.AllClasses.FunctionIcon");
	return Icon;
}

void UK2Node_EmitEventFromInterface::PostReconstructNode()
{
	Super::PostReconstructNode();
}

void UK2Node_EmitEventFromInterface::UpdateSlotExtern(FEventSlot Slot)
{
	FScopedTransaction Transaction(LOCTEXT("UpdateSlotTransaction", "UpdateSlotTransaction"));
	this->Event = Slot;

	this->RefreshDefaultValue();
}

void UK2Node_EmitEventFromInterface::RefreshDefaultValue()
{
	if (auto Pin = this->GetEventPin())
	{
		Pin->DefaultValue =
			FString::Printf(TEXT("(EventName=\"%s\")"), *this->Event.Name().ToString());
	}
}


#if WITH_EDITOR
void UK2Node_EmitEventFromInterface::PostEditChangeProperty(FPropertyChangedEvent& PropertyEvent)
{
	Super::PostEditChangeProperty(PropertyEvent);

	this->RefreshDefaultValue();
	this->ReconstructNode();
}
#endif

#undef LOCTEXT_NAMESPACE
