#include "Widgets/SGraphPinEventSlotCombo.h"
#include "Kismet/K2Node_EmitEventFromInterface.h"
#include "PropertyCustomization/EventSlot/SEventSlotCombo.h"

SGraphPinEventSlotCombo::SGraphPinEventSlotCombo()
{

}

void SGraphPinEventSlotCombo::Construct(
	const FArguments& InArgs,
	UK2Node_EmitEventFromInterface* InNode)
{
	this->Node = InNode;
	SGraphPin::Construct(SGraphPin::FArguments(), InNode->GetEventPin());	
}

TSharedRef<SWidget> SGraphPinEventSlotCombo::GetDefaultValueWidget()
{
	if (this->Node.IsValid())
	{
		return SNew(SEventSlotCombo)
			.OnSlotChanged_UObject(this->Node.Get(), &UK2Node_EmitEventFromInterface::UpdateSlotExtern)
			.Slot(this->Node->GetSlot());
	}	
	else
	{
		return SGraphPin::GetDefaultValueWidget();
	}
}