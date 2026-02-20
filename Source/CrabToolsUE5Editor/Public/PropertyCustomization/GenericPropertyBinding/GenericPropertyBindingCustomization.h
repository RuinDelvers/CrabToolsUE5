#pragma once

#include "IDetailCustomization.h"

class FGenericPropertyBindingCustomization : public IDetailCustomization
{

private:

	//TSharedPtr<IPropertyHandle> CachedPropertyHandle;
	//TSharedPtr<SGenericPropertyBindingPicker> SlotWidget;

public:

	static TSharedRef<IDetailCustomization> MakeInstance();

public:

	virtual void PendingDelete() override;
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
	virtual void CustomizeDetails(const TSharedPtr<IDetailLayoutBuilder>& DetailBuilder) override;
};
