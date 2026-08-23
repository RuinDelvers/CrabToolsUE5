#pragma once

#include "Input/Reply.h"
#include "Widgets/SWidget.h"
#include "Widgets/SCompoundWidget.h"
#include "PropertyCustomization/GenericPropertyBinding/SGenericPropertyPath.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

class IPropertyHandle;
class SMenuAnchor;
class SButton;
class SComboButton;


/**
 * 
 */
class CRABTOOLSUE5EDITOR_API SGenericPropertyCustomizer : public SCompoundWidget
{

private:

	TSharedPtr<STextBlock> ComboButtonText;
	TSharedPtr<SComboButton> ComboButton;
	TSharedPtr<IPropertyHandle> PropertyHandle;	

public:

	SLATE_BEGIN_ARGS(SGenericPropertyCustomizer)
		{}

	SLATE_END_ARGS();

	SGenericPropertyCustomizer();

	void Construct(const FArguments& InArgs, TSharedPtr<IPropertyHandle> InitProp);

private:

	FText GetPathText() const;
	FText GetDisplayText() const;
	TSharedRef<SWidget> OnGetMenuContent();
	FEdGraphPinType GetPinTypeForProp() const;
	void OnPropertyValueChanged();

	void OnSelectorClosed();

	//EVisibility GetClearVisibility() const;
	FReply ClearValue();

	TSharedPtr<SWidget> ConstructEditableWidget();
	TSharedPtr<SWidget> ConstructConstWidget();
};
