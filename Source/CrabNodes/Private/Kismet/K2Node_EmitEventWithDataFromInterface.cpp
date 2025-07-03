// Copyright Epic Games, Inc. All Rights Reserved.

#include "Kismet/K2Node_EmitEventWithDataFromInterface.h"

#include "Containers/EnumAsByte.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraphSchema_K2.h"
#include "Engine/MemberReference.h"
#include "Internationalization/Internationalization.h"
#include "K2Node_CallFunction.h"
#include "Kismet2/CompilerResultsLog.h"
#include "KismetCompiler.h"
#include "Misc/AssertionMacros.h"
#include "Templates/Casts.h"
#include "UObject/NameTypes.h"
#include "UObject/Object.h"
#include "StateMachine/StateMachineInterface.h"
#include "StateMachine/HelperLibrary.h"

class UBlueprint;

#define LOCTEXT_NAMESPACE "K2Node_EmitEventWithDataFromInterface"

namespace EmitEventWithDataFromInterfaceHelper
{
	const FName DataPinName = "Data";
}

UK2Node_EmitEventWithDataFromInterface::UK2Node_EmitEventWithDataFromInterface(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeTooltip = LOCTEXT("NodeTooltip", "Emits an event to a State Machine from a Data Table.");
}

void UK2Node_EmitEventWithDataFromInterface::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	// Add Data Pin
	UEdGraphPin* DataPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UObject::StaticClass(), EmitEventWithDataFromInterfaceHelper::DataPinName);
	SetPinToolTip(*DataPin, LOCTEXT("DataPinDescription", "The data to be passed with the event."));

}

UEdGraphPin* UK2Node_EmitEventWithDataFromInterface::GetDataPin() const
{
	UEdGraphPin* Pin = FindPinChecked(EmitEventWithDataFromInterfaceHelper::DataPinName);
	check(Pin->Direction == EGPD_Input);
	return Pin;
}

FText UK2Node_EmitEventWithDataFromInterface::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (TitleType == ENodeTitleType::MenuTitle)
	{
		return LOCTEXT("ListViewTitle", "Emit Event With Data From Interface");
	}
	else
	{
		return LOCTEXT("Interface_Title", "Emit Event With Data From Interface");
	}
}

void UK2Node_EmitEventWithDataFromInterface::ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	// Skip the standard node expansion, since it uses the wrong function.
    UK2Node::ExpandNode(CompilerContext, SourceGraph);
    
	UEdGraphPin* OriginalInterfaceInPin = GetInterfacePin();
	UEdGraphPin* OriginalStateMachineInPin = GetStateMachinePin();

    UStateMachineInterface* Table = (OriginalInterfaceInPin != NULL) ? Cast<UStateMachineInterface>(OriginalInterfaceInPin->DefaultObject) : NULL;
    if((nullptr == OriginalInterfaceInPin) || (0 == OriginalInterfaceInPin->LinkedTo.Num() && nullptr == Table))
    {
        CompilerContext.MessageLog.Error(*LOCTEXT("NoInterface_Error", "EmitEventWithDataFromInterface must have a Interface specified.").ToString(), this);
        // we break exec links so this is the only error we get
        BreakAllNodeLinks();
        return;
    }

	// FUNCTION NODE
	const FName FunctionName = GET_FUNCTION_NAME_CHECKED(UStateMachineHelperLibrary, EmitEventWithData);
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
		CompilerContext.MessageLog.Error(*LOCTEXT("NoStateMachine_Error", "EmitEventWithDataFromInterface must have a StateMachine specified.").ToString(), this);
		// we break exec links so this is the only error we get
		BreakAllNodeLinks();
		return;
	}

	UEdGraphPin* EventInPin = EmitEventFromInterfaceFunction->FindPinChecked(TEXT("EName"));
	CompilerContext.MovePinLinksToIntermediate(*GetEventPin(), *EventInPin);

	UEdGraphPin* DataInPin = EmitEventFromInterfaceFunction->FindPinChecked(TEXT("Data"));
	CompilerContext.MovePinLinksToIntermediate(*GetDataPin(), *DataInPin);

	// Get some pins to work with
    UEdGraphPin* FunctionThenPin = EmitEventFromInterfaceFunction->GetThenPin();
                
    // Hook up outputs
    CompilerContext.MovePinLinksToIntermediate(*GetThenPin(), *FunctionThenPin);

	BreakAllNodeLinks();
}

void UK2Node_EmitEventWithDataFromInterface::PostReconstructNode()
{
	Super::PostReconstructNode();
}

#undef LOCTEXT_NAMESPACE
