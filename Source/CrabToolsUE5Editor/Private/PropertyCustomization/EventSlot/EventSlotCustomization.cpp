#include "PropertyCustomization/EventSlot/EventSlotCustomization.h"
#include "PropertyCustomization/EventSlot/SEventSlotCombo.h"
#include "PropertyCustomization/EventSlot/SEventSlotPicker.h"
#include "StateMachine/DataStructures.h"
#include "IDetailChildrenBuilder.h"
#include "PropertyHandle.h"
#include "DetailWidgetRow.h"


TSharedRef<IPropertyTypeCustomization> FEventSlotCustomization::MakeInstance()
{
	return MakeShareable(new FEventSlotCustomization());
}

void FEventSlotCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	this->CachedPropertyHandle = StructPropertyHandle;

	HeaderRow.NameContent()
		[
			StructPropertyHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		[
			SNew(SEventSlotCombo)
				.PropertyHandle(this->CachedPropertyHandle)
		];
}

void FEventSlotCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	
}