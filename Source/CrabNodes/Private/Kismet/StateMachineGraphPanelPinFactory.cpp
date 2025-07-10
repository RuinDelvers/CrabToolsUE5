#include "Kismet/StateMachineGraphPanelPinFactory.h"
#include "EdGraphSchema_K2.h"
#include "Widgets/SGraphPinSMEventName.h"

TSharedPtr<class SGraphPin> FStateMachineGraphPanelPinFactory::CreatePin(class UEdGraphPin* InPin) const
{
	if (InPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Name)
	{
		UObject* Outer = InPin->GetOuter();
		if (auto Node = Cast<UK2Node_EmitEventBase>(Outer))
		{
			return SNew(SGraphPinSMEventName, InPin, Node);
		}
	}

	return nullptr;
}