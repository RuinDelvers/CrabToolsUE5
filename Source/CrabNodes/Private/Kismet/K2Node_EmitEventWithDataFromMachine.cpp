// Copyright Epic Games, Inc. All Rights Reserved.

#include "Kismet/K2Node_EmitEventWithDataFromMachine.h"


#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "Containers/EnumAsByte.h"
#include "Containers/Map.h"
#include "Containers/UnrealString.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraph/EdGraphSchema.h"
#include "EdGraphSchema_K2.h"
#include "EditorCategoryUtils.h"
#include "Engine/MemberReference.h"
#include "HAL/PlatformMath.h"
#include "Internationalization/Internationalization.h"
#include "K2Node_CallFunction.h"
#include "K2Node_IfThenElse.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/CompilerResultsLog.h"
#include "KismetCompiler.h"
#include "Math/Color.h"
#include "Misc/AssertionMacros.h"
#include "Styling/AppStyle.h"
#include "Templates/Casts.h"
#include "UObject/Class.h"
#include "UObject/NameTypes.h"
#include "UObject/Object.h"
#include "UObject/ObjectPtr.h"
#include "UObject/UObjectBaseUtility.h"
#include "UObject/UnrealNames.h"
#include "UObject/WeakObjectPtr.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "StateMachine/StateMachineInterface.h"
#include "StateMachine/HelperLibrary.h"

class UBlueprint;

#define LOCTEXT_NAMESPACE "K2Node_EmitEventWithDataFromMachine"

namespace EmitEventWithDataFromMachineHelper
{
	const FName DataPinName = "Data";
}

UK2Node_EmitEventWithDataFromMachine::UK2Node_EmitEventWithDataFromMachine(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeTooltip = LOCTEXT("NodeTooltip", "Emits an event to a State Machine from a Data Table.");
}

void UK2Node_EmitEventWithDataFromMachine::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	// Add Data Pin
	UEdGraphPin* DataPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UObject::StaticClass(), EmitEventWithDataFromMachineHelper::DataPinName);
	SetPinToolTip(*DataPin, LOCTEXT("DataPinDescription", "The data to be passed with the event."));

}

UEdGraphPin* UK2Node_EmitEventWithDataFromMachine::GetDataPin() const
{
	UEdGraphPin* Pin = FindPinChecked(EmitEventWithDataFromInterfaceHelper::DataPinName);
	check(Pin->Direction == EGPD_Input);
	return Pin;
}

FText UK2Node_EmitEventWithDataFromMachine::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (TitleType == ENodeTitleType::MenuTitle)
	{
		return LOCTEXT("ListViewTitle", "Emit Event With Data From Machine");
	}
	else
	{
		return NSLOCTEXT("K2Node", "Interface_Title", "Emit Event With Data From Machine");
	}
}

void UK2Node_EmitEventWithDataFromMachine::ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	// Skip the standard node expansion, since it uses the wrong function.
    UK2Node::ExpandNode(CompilerContext, SourceGraph);
    
	UEdGraphPin* OriginalStateMachineInPin = GetStateMachinePin();

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
		CompilerContext.MessageLog.Error(*LOCTEXT("EmitEventWithDataFromInterfaceNoInterface_Error", "EmitEventWithDataFromInterface must have a StateMachine specified.").ToString(), this);
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

void UK2Node_EmitEventWithDataFromMachine::PostReconstructNode()
{
	Super::PostReconstructNode();
}


#undef LOCTEXT_NAMESPACE