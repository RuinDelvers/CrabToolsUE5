#pragma once

#include "IPropertyTypeCustomization.h"

class SEventSlotPicker;

class FEventSlotCustomization : public IPropertyTypeCustomization
{

private:

	TSharedPtr<IPropertyHandle> CachedPropertyHandle;
	TSharedPtr<SEventSlotPicker> SlotWidget;

public:

	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

public:

	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
};