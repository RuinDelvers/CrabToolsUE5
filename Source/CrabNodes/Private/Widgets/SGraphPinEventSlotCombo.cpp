#include "Widgets/SGraphPinEventSlotCombo.h"
#include "EdGraph/EdGraphPin.h"
#include "PropertyCustomization/EventSlot/SEventSlotCombo.h"

SGraphPinEventSlotCombo::SGraphPinEventSlotCombo()
{

}

void SGraphPinEventSlotCombo::Construct(
	const FArguments& InArgs,
	UEdGraphPin* InPin)
{
	SGraphPin::Construct(SGraphPin::FArguments(), InPin);	
}

TSharedRef<SWidget> SGraphPinEventSlotCombo::GetDefaultValueWidget()
{
	FString StructName;
	FEventSlot TestSlot;

	FEventSlot::StaticStruct()->ImportText(*this->GraphPinObj->DefaultValue, &TestSlot, nullptr, 0, nullptr, StructName);

	return SNew(SEventSlotCombo)
		.Slot(TestSlot)
		.OnSlotChanged_Lambda([this](const FEventSlot Event)
			{
				FString Output;
				FEventSlot::StaticStruct()->ExportText(Output, &Event, nullptr, nullptr, 0, nullptr);
				this->GraphPinObj->DefaultValue = Output;
			});
}