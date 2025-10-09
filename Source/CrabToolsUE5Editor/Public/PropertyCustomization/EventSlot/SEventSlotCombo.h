#pragma once

#include "Input/Reply.h"
#include "PropertyCustomization/EventSlot/SEventSlotChip.h"
#include "Widgets/SWidget.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "StateMachine/DataStructures.h"
//#include "GameplayTagContainer.h"
//#include "SGameplayTagChip.h"

class IPropertyHandle;
class SMenuAnchor;
class SButton;
class SComboButton;
class SEventSlotPicker;

/**
 * Widget for editing a Gameplay Tag.
 */
class SEventSlotCombo : public SCompoundWidget
{
	SLATE_DECLARE_WIDGET_API(SEventSlotCombo, SCompoundWidget, CRABTOOLSUE5EDITOR_API)

public:

	DECLARE_DELEGATE_OneParam(FOnSlotChanged, const FEventSlot /*Tag*/)

private:

	TSlateAttribute<FEventSlot> SlotAttribute;
	FString Filter;
	FString SettingsName;
	FOnSlotChanged OnSlotChanged;
	TSharedPtr<SComboButton> ComboButton;
	TSharedPtr<SMenuAnchor> MenuAnchor;
	TSharedPtr<SEventSlotPicker> SlotPicker;
	TSharedPtr<IPropertyHandle> PropertyHandle;
	bool bIsReadOnly = false;
	bool bHasMultipleValues = false;

public:

	SLATE_BEGIN_ARGS(SEventSlotCombo)
		: _Filter()
		, _ReadOnly(false)
		, _EnableNavigation(false)
		, _PropertyHandle(nullptr)
		{}
		// Comma delimited string of tag root names to filter by
		SLATE_ARGUMENT(FString, Filter)

		// The name that will be used for the tag picker settings file
		SLATE_ARGUMENT(FString, SettingsName)

		// Flag to set if the list is read only
		SLATE_ARGUMENT(bool, ReadOnly)

		// If true, allow button navigation behavior
		SLATE_ARGUMENT(bool, EnableNavigation)

		// Tags to edit
		SLATE_ATTRIBUTE(FEventSlot, Slot)

		// If set, the tag is read from the property, and the property is changed when tag is edited. 
		SLATE_ARGUMENT(TSharedPtr<IPropertyHandle>, PropertyHandle)

		// Callback for when button body is pressed with LMB+Ctrl
		SLATE_EVENT(SEventSlotChip::FOnNavigate, OnNavigate)

		// Callback for when button body is pressed with RMB
		SLATE_EVENT(SEventSlotChip::FOnMenu, OnMenu)

		// Called when a tag status changes
		SLATE_EVENT(FOnSlotChanged, OnSlotChanged)
	SLATE_END_ARGS();

	SEventSlotCombo();

	void Construct(const FArguments& InArgs);

private:

	bool IsValueEnabled() const;
	void OnMenuOpenChanged(const bool bOpen) const;
	TSharedRef<SWidget> OnGetMenuContent();
	bool ShowClearButton() const;
	FText GetText() const;
	FText GetToolTipText() const;
	bool IsSelected() const;
	FReply OnClearPressed();
	void OnClearTag();
	FReply OnEditTag() const;
	FReply OnTagMenu(const FPointerEvent& MouseEvent);
};
