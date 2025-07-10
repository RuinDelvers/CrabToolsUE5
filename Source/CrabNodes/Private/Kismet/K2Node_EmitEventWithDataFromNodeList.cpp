// Copyright Epic Games, Inc. All Rights Reserved.

#include "Kismet/K2Node_EmitEventWithDataFromNodeList.h"


#include "Containers/EnumAsByte.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraphSchema_K2.h"
#include "Engine/MemberReference.h"
#include "Internationalization/Internationalization.h"
#include "K2Node_CallFunction.h"
#include "KismetCompiler.h"
#include "Misc/AssertionMacros.h"
#include "UObject/NameTypes.h"
#include "UObject/Object.h"

class UBlueprint;

#define LOCTEXT_NAMESPACE "K2Node_EmitEventWithDataFromNodeList"

namespace EmitEventWithDataFromNodeListHelper
{
	const FName DataPinName = "Data";
}

UK2Node_EmitEventWithDataFromNodeList::UK2Node_EmitEventWithDataFromNodeList(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeTooltip = LOCTEXT("NodeTooltip", "Emits an event to this node's owner with data.");
}

void UK2Node_EmitEventWithDataFromNodeList::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	// Add Data Pin
	UEdGraphPin* DataPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UObject::StaticClass(), EmitEventWithDataFromNodeListHelper::DataPinName);
	SetPinToolTip(*DataPin, LOCTEXT("DataPinDescription", "The data to be passed with the event."));

}

UEdGraphPin* UK2Node_EmitEventWithDataFromNodeList::GetDataPin() const
{
	UEdGraphPin* Pin = FindPinChecked(EmitEventWithDataFromNodeListHelper::DataPinName);
	check(Pin->Direction == EGPD_Input);
	return Pin;
}

FText UK2Node_EmitEventWithDataFromNodeList::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (TitleType == ENodeTitleType::MenuTitle)
	{
		return LOCTEXT("ListViewTitle", "Emit Event With Data");
	}
	else
	{
		return LOCTEXT("Interface_Title", "Emit Event With Data");
	}
}

void UK2Node_EmitEventWithDataFromNodeList::ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	// FUNCTION NODE
	const FName FunctionName = GET_FUNCTION_NAME_CHECKED(UStateNode, EmitEventWithData);
	UK2Node_CallFunction* EmitEventFromNodeListFunction = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	EmitEventFromNodeListFunction->FunctionReference.SetExternalMember(FunctionName, UStateNode::StaticClass());
	EmitEventFromNodeListFunction->AllocateDefaultPins();
	CompilerContext.MovePinLinksToIntermediate(*GetExecPin(), *(EmitEventFromNodeListFunction->GetExecPin()));

	// Connect the input of our EmitEventFromNodeList to the Input of our Function pin
	UEdGraphPin* SelfInPin = EmitEventFromNodeListFunction->FindPinChecked(TEXT("self"));
	CompilerContext.MovePinLinksToIntermediate(*GetSelfPin(), *SelfInPin);

	UEdGraphPin* EventInPin = EmitEventFromNodeListFunction->FindPinChecked(TEXT("EName"));
	CompilerContext.MovePinLinksToIntermediate(*GetEventPin(), *EventInPin);

	UEdGraphPin* DataInPin = EmitEventFromNodeListFunction->FindPinChecked(TEXT("Data"));
	CompilerContext.MovePinLinksToIntermediate(*GetDataPin(), *DataInPin);

	// Get some pins to work with
	UEdGraphPin* FunctionThenPin = EmitEventFromNodeListFunction->GetThenPin();

	// Hook up outputs
	CompilerContext.MovePinLinksToIntermediate(*GetThenPin(), *FunctionThenPin);

	BreakAllNodeLinks();
}

void UK2Node_EmitEventWithDataFromNodeList::PostReconstructNode()
{
	Super::PostReconstructNode();
}

#undef LOCTEXT_NAMESPACE
