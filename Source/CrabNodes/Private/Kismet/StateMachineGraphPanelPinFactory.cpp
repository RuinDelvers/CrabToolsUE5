#include "Kismet/StateMachineGraphPanelPinFactory.h"
#include "Kismet/K2Node_EmitEventFromInterface.h"
#include "Kismet/K2Node_EmitEventFromMachine.h"
#include "Kismet/K2Node_EmitEventWithDataFromInterface.h"
#include "EdGraphSchema_K2.h"
#include "K2Node_CallFunction.h"
#include "Widgets/SGraphPinSMEventName.h"
#include "SGraphPinNameList.h"
#include "StateMachine/StateMachineInterface.h"

TSharedPtr<class SGraphPin> FStateMachineGraphPanelPinFactory::CreatePin(class UEdGraphPin* InPin) const
{
	if (InPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Name)
	{
		UObject* Outer = InPin->GetOuter();

		// Create drop down combo boxes for Interface and CurveTable RowName pins
		const UEdGraphPin* InterfacePin = nullptr;

		if (Outer->IsA(UK2Node_EmitEventFromInterface::StaticClass()))
		{
			const auto GetInterfaceRowNode = CastChecked<UK2Node_EmitEventFromInterface>(Outer);
			InterfacePin = GetInterfaceRowNode->GetInterfacePin();
		}
		else if (Outer->IsA(UK2Node_EmitEventWithDataFromInterface::StaticClass()))
		{
			const auto GetInterfaceRowNode = CastChecked<UK2Node_EmitEventWithDataFromInterface>(Outer);
			InterfacePin = GetInterfaceRowNode->GetInterfacePin();
		}
		else if (Outer->IsA(UK2Node_EmitEventFromMachine::StaticClass()))
		{
			const auto GetInterfaceRowNode = CastChecked<UK2Node_EmitEventFromMachine>(Outer);
			InterfacePin = GetInterfaceRowNode->GetInterfacePin();
		}

		if (InterfacePin)
		{
			if (InterfacePin->DefaultObject != nullptr && (InterfacePin->LinkedTo.Num() == 0))
			{
				if (auto Interface = Cast<UStateMachineInterface>(InterfacePin->DefaultObject))
				{
					// The pin for an FName for an event.
					auto Pin = SNew(SGraphPinSMEventName, InPin, Interface);

					if (auto NodeType1 = Cast<UK2Node_EmitEventFromInterface>(Outer))
					{
						NodeType1->OnInterfaceChanged.AddSP(Pin, &SGraphPinSMEventName::RefreshNameList);
					}
					else if (auto NodeType2 = Cast<UK2Node_EmitEventWithDataFromInterface>(Outer))
					{
						NodeType2->OnInterfaceChanged.AddSP(Pin, &SGraphPinSMEventName::RefreshNameList);
					}

					return Pin;
				}
			} else if (InterfacePin->LinkedTo.Num() > 0) {
				if (auto NodeType = Cast<UK2Node_EmitEventFromMachine>(Outer))
				{
					auto Pin = SNew(SGraphPinSMEventName, InPin, nullptr, NodeType->GetSMClass());
					NodeType->OnInterfaceChanged.AddSP(Pin, &SGraphPinSMEventName::RefreshNameList);
					return Pin;
				}
			}
		}
	}

	return nullptr;
}