#include "Kismet/StateMachineGraphPanelPinFactory.h"
#include "EdGraphSchema_K2.h"
#include "Widgets/SGraphPinSMEventName.h"
#include "Widgets/SGraphPinEventSlotCombo.h"
#include "StateMachine/DataStructures.h"
#include "Kismet/K2Node_EmitEventFromNodeList.h"

TSharedPtr<class SGraphPin> FStateMachineGraphPanelPinFactory::CreatePin(class UEdGraphPin* InPin) const
{
	TSharedPtr<SGraphPin> OutPin;

	if (auto EventNode = Cast<UK2Node_EmitEventFromNodeList>(InPin->GetOuter()))
	{
		if (InPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Name)
		{
			UObject* Outer = InPin->GetOuter();
			if (auto Node = Cast<UK2Node_EmitEventBase>(Outer))
			{
				OutPin = SNew(SGraphPinSMEventName, InPin, Node);
			}
		}
	}
	else
	{
		if (InPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct)
		{
			if (InPin->PinType.PinSubCategoryObject == FEventSlot::StaticStruct())
			{
				OutPin = SNew(SGraphPinEventSlotCombo, InPin);
			}
		}
	}

	return OutPin;
}