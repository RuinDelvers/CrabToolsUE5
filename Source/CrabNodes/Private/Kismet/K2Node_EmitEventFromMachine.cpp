// Copyright Epic Games, Inc. All Rights Reserved.

#include "Kismet/K2Node_EmitEventFromMachine.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "Containers/EnumAsByte.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraphSchema_K2.h"
#include "Internationalization/Internationalization.h"
#include "K2Node_CallFunction.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "KismetCompiler.h"
#include "Misc/AssertionMacros.h"
#include "Templates/Casts.h"
#include "UObject/Class.h"
#include "UObject/NameTypes.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "StateMachine/StateMachine.h"
#include "StateMachine/StateMachineBlueprintGeneratedClass.h"
#include "StateMachine/HelperLibrary.h"

class UBlueprint;

#define LOCTEXT_NAMESPACE "K2Node_EmitEventFromMachine"

namespace EmitEventFromMachineHelper
{
	const FName StateMachinePinName = "StateMachine";
	const FName EventPinName = "Event";
}

UK2Node_EmitEventFromMachine::UK2Node_EmitEventFromMachine(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeTooltip = LOCTEXT("NodeTooltip", "Emits an event to a State Machine from a State Machine Type.");
}

void UK2Node_EmitEventFromMachine::AllocateDefaultPins()
{
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	// Add execution pins
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	UEdGraphPin* RowFoundPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);

	// Add State Machine Pin
	UEdGraphPin* StateMachinePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UStateMachine::StaticClass(), EmitEventFromMachineHelper::StateMachinePinName);
	SetPinToolTip(*StateMachinePin, LOCTEXT("StateMachinePinDescription", "The State Machine to send an event to"));

	// Event Name Pin
	UEdGraphPin* EventPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Name, EmitEventFromMachineHelper::EventPinName);
	SetPinToolTip(*EventPin, LOCTEXT("EventPinDescription", "The event name to send to the state machine."));

	Super::AllocateDefaultPins();
}

void UK2Node_EmitEventFromMachine::SetPinToolTip(UEdGraphPin& MutatablePin, const FText& PinDescription) const
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

void UK2Node_EmitEventFromMachine::RefreshEventOptions()
{
	// When the Interface pin gets a new value assigned, we need to update the Slate UI so that SGraphNodeCallParameterCollectionFunction will update the ParameterName drop down
	UEdGraph* Graph = GetGraph();
	Graph->NotifyNodeChanged(this);

	auto Events = this->GetEventSet();
	this->OnEventSetChanged.Broadcast(Events);
}

void UK2Node_EmitEventFromMachine::OnInterfaceRowListChanged(TSubclassOf<UStateMachine> Interface)
{
	UEdGraphPin* InterfacePin = GetInterfacePin();
	if (Interface && InterfacePin && Interface == InterfacePin->DefaultObject)
	{
		UEdGraphPin* EventPin = GetEventPin();
		const bool TryRefresh = EventPin && !EventPin->LinkedTo.Num();
		const FName CurrentName = EventPin ? FName(*EventPin->GetDefaultAsString()) : NAME_None;

		if (TryRefresh && EventPin)// && !Interface->GetEvents().Contains(CurrentName))
		{
			if (auto SMGM = Cast<UStateMachineBlueprintGeneratedClass>(this->GetSMClass()))
			{
				if (UBlueprint* BP = GetBlueprint())
				{
					FBlueprintEditorUtils::MarkBlueprintAsModified(BP);
				}
			}
		}
	}
}

void UK2Node_EmitEventFromMachine::ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins) 
{
	Super::ReallocatePinsDuringReconstruction(OldPins);

	if (UEdGraphPin* InterfacePin = GetInterfacePin(&OldPins))
	{
		if (auto Interface = this->GetSMClass())
		{
			PreloadObject(Interface);
		}
	}
}

TSubclassOf<UStateMachine> UK2Node_EmitEventFromMachine::GetSMClass() const
{
	auto Pin = this->GetStateMachinePin();

	if (Pin->LinkedTo.Num() == 0)
	{
		return nullptr;
	}
	else
	{
		if (Pin->LinkedTo[0]->GetPrimaryTerminalType().TerminalSubCategoryObject.IsValid())
		{
			auto Obj = Pin->LinkedTo[0]->GetPrimaryTerminalType().TerminalSubCategoryObject.Get();
			TSubclassOf<UStateMachine> C;
			C = Cast<UClass>(Obj);

			return C;
		}
	}

	return nullptr;
}

void UK2Node_EmitEventFromMachine::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
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

FText UK2Node_EmitEventFromMachine::GetMenuCategory() const
{
	return NSLOCTEXT("K2Node_EmitEventFromMachine", "Category", "State Machine");
}

bool UK2Node_EmitEventFromMachine::IsConnectionDisallowed(const UEdGraphPin* MyPin, const UEdGraphPin* OtherPin, FString& OutReason) const
{
	return false;
}

void UK2Node_EmitEventFromMachine::PinDefaultValueChanged(UEdGraphPin* ChangedPin) 
{
	if (ChangedPin && ChangedPin->PinName == EmitEventFromMachineHelper::StateMachinePinName)
	{
		UEdGraphPin* EventPin = GetEventPin();

		if (EventPin)
		{
			RefreshEventOptions();
		}
	}
}

FText UK2Node_EmitEventFromMachine::GetTooltipText() const
{
	return NodeTooltip;
}

UEdGraphPin* UK2Node_EmitEventFromMachine::GetThenPin()const
{
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	UEdGraphPin* Pin = FindPinChecked(UEdGraphSchema_K2::PN_Then);
	check(Pin->Direction == EGPD_Output);
	return Pin;
}

UEdGraphPin* UK2Node_EmitEventFromMachine::GetInterfacePin(const TArray<UEdGraphPin*>* InPinsToSearch) const
{
	const TArray<UEdGraphPin*>* PinsToSearch = InPinsToSearch ? InPinsToSearch : &Pins;
    
	UEdGraphPin* Pin = nullptr;
	for (UEdGraphPin* TestPin : *PinsToSearch)
	{
		if (TestPin && TestPin->PinName == EmitEventFromMachineHelper::StateMachinePinName)
		{
			Pin = TestPin;
			break;
		}
	}
	check(Pin == nullptr || Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UK2Node_EmitEventFromMachine::GetEventPin() const
{
	UEdGraphPin* Pin = FindPinChecked(EmitEventFromMachineHelper::EventPinName);
	check(Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UK2Node_EmitEventFromMachine::GetStateMachinePin() const
{
	UEdGraphPin* Pin = FindPinChecked(EmitEventFromMachineHelper::StateMachinePinName);
	check(Pin->Direction == EGPD_Input);
	return Pin;
}

FText UK2Node_EmitEventFromMachine::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (TitleType == ENodeTitleType::MenuTitle)
	{
		return LOCTEXT("ListViewTitle", "Emit Event From Machine");
	}
	else
	{
		return NSLOCTEXT("K2Node", "Interface_Title", "Emit Event From Machine");	
	}
}

void UK2Node_EmitEventFromMachine::ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
    Super::ExpandNode(CompilerContext, SourceGraph);
    
	UEdGraphPin* OriginalStateMachineInPin = GetStateMachinePin();

	// FUNCTION NODE
	const FName FunctionName = GET_FUNCTION_NAME_CHECKED(UStateMachineHelperLibrary, EmitEvent);
	UK2Node_CallFunction* EmitEventFromInterfaceFunction = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	EmitEventFromInterfaceFunction->FunctionReference.SetExternalMember(FunctionName, UStateMachineHelperLibrary::StaticClass());
	EmitEventFromInterfaceFunction->AllocateDefaultPins();
    CompilerContext.MovePinLinksToIntermediate(*GetExecPin(), *(EmitEventFromInterfaceFunction->GetExecPin()));

	// Connect the input of our EmitEventFromInterface to the Input of our Function pin
    UEdGraphPin* StateMachineInPin = EmitEventFromInterfaceFunction->FindPinChecked(TEXT("Obj"));

	// This check is omitted currently as the compilation of this node have simultaneously with the
	// SM it references. This causes the event set to always be empty, and thus this will always fire
	// at the first compilation upon loading the editor.
	/*
	if (!this->CheckValidEvent(CompilerContext.MessageLog))
	{
		BreakAllNodeLinks();
		return;
	}
	*/

	if(OriginalStateMachineInPin->LinkedTo.Num() > 0)
	{
		// Copy the connection
		CompilerContext.MovePinLinksToIntermediate(*OriginalStateMachineInPin, *StateMachineInPin);
	}
	else
	{
		CompilerContext.MessageLog.Error(*LOCTEXT("NoStateMachine_Error", "@@ EmitEventFromMachine must have a StateMachine specified.").ToString(), this);
		// we break exec links so this is the only error we get
		BreakAllNodeLinks();
		return;
	}

	UEdGraphPin* EventInPin = EmitEventFromInterfaceFunction->FindPinChecked(TEXT("EName"));
	CompilerContext.MovePinLinksToIntermediate(*GetEventPin(), *EventInPin);

	// Get some pins to work with
    UEdGraphPin* FunctionThenPin = EmitEventFromInterfaceFunction->GetThenPin();
                
    // Hook up outputs
    CompilerContext.MovePinLinksToIntermediate(*GetThenPin(), *FunctionThenPin);

	BreakAllNodeLinks();
}

FSlateIcon UK2Node_EmitEventFromMachine::GetIconAndTint(FLinearColor& OutColor) const
{
	OutColor = GetNodeTitleColor();
	static FSlateIcon Icon(FAppStyle::GetAppStyleSetName(), "Kismet.AllClasses.FunctionIcon");
	return Icon;
}

void UK2Node_EmitEventFromMachine::PostReconstructNode()
{
	Super::PostReconstructNode();
}

void UK2Node_EmitEventFromMachine::EarlyValidation(class FCompilerResultsLog& MessageLog) const
{
	Super::EarlyValidation(MessageLog);

	const UEdGraphPin* InterfacePin = GetInterfacePin();
	const UEdGraphPin* EventPin = GetEventPin();

	if (!InterfacePin || !EventPin)
	{
		MessageLog.Error(*LOCTEXT("MissingPins", "Missing pins in @@").ToString(), this);
		return;
	}

	if (InterfacePin->LinkedTo.Num() == 0)
	{
		//const UStateMachineInterface* Interface = Cast<UStateMachineInterface>(InterfacePin->DefaultObject);
		const auto Interface = this->GetSMClass();
		if (!Interface)
		{
			MessageLog.Error(*LOCTEXT("NoInterface", "No Interface in @@").ToString(), this);
			return;
		}

		if (!EventPin->LinkedTo.Num())
		{
			const FName CurrentName = FName(*EventPin->GetDefaultAsString());
			if (auto SMGC = Cast<UStateMachineBlueprintGeneratedClass>(Interface))
			{
				if (!SMGC->GetTotalEventSet().Contains(CurrentName))
				{
					const FString Msg = FText::Format(
						LOCTEXT("WrongEventFmt", "'{0}' is not an event for SM Class '{1}'. @@"),
						FText::FromString(CurrentName.ToString()),
						FText::FromString(GetFullNameSafe(Interface))
					).ToString();
					MessageLog.Error(*Msg, this);
					return;
				}
			}
		}
	}
}

void UK2Node_EmitEventFromMachine::PreloadRequiredAssets()
{
	if (UEdGraphPin* InterfacePin = GetStateMachinePin())
	{
		if (auto Interface = this->GetSMClass())
		{
			PreloadObject(Interface);
		}
	}
	return Super::PreloadRequiredAssets();
}

void UK2Node_EmitEventFromMachine::NotifyPinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::NotifyPinConnectionListChanged(Pin);

	if (Pin == GetInterfacePin())
	{
		if (Pin->LinkedTo.Num() > 0)
		{
			RefreshEventOptions();
		}
	}
}

TSet<FName> UK2Node_EmitEventFromMachine::GetEventSet() const
{
	if (auto SMClass = this->GetSMClass())
	{
		if (auto BPGC = Cast<UStateMachineBlueprintGeneratedClass>(SMClass))
		{
			return BPGC->GetTotalEventSet();
		}		
	}

	return {};
}

#undef LOCTEXT_NAMESPACE
