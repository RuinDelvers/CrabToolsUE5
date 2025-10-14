#include "PropertyCustomization/EventSlot/SEventSlotCombo.h"
#include "PropertyCustomization/EventSlot/SEventSlotPicker.h"
#include "Styles/EventSlotStyle.h"
#include "DetailLayoutBuilder.h"
#include "Framework/Application/SlateApplication.h"
#include "ScopedTransaction.h"
#include "Editor.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Widgets/Input/SComboButton.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"


#define LOCTEXT_NAMESPACE "GameplayTagCombo"

/*
SLATE_IMPLEMENT_WIDGET(SEventSlotCombo)
void SEventSlotCombo::PrivateRegisterAttributes(FSlateAttributeInitializer& AttributeInitializer)
{
	SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(AttributeInitializer, "EventSlot", SlotAttribute, EInvalidateWidgetReason::Layout);
}
*/

SEventSlotCombo::SEventSlotCombo()
	//: SlotAttribute(*this)
{
	
}

void SEventSlotCombo::Construct(const FArguments& InArgs)
{
	//this->SlotAttribute.Assign(*this, InArgs._Slot);
	this->Filter = InArgs._Filter;
	this->SettingsName = InArgs._SettingsName;
	this->OnSlotChanged = InArgs._OnSlotChanged;
	this->PropertyHandle = InArgs._PropertyHandle;
	this->InlineSlot = InArgs._Slot;

	ChildSlot
		[
			SNew(SHorizontalBox) // Extra box to make the combo hug the chip

				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SAssignNew(ComboButton, SComboButton)
						.ComboButtonStyle(FEventSlotStyle::Get(), "EventSlot.ComboButton")
						.HasDownArrow(true)
						.ContentPadding(1)
						.IsEnabled(this, &SEventSlotCombo::IsValueEnabled)
						.Clipping(EWidgetClipping::OnDemand)
						.OnMenuOpenChanged(this, &SEventSlotCombo::OnMenuOpenChanged)
						.OnGetMenuContent(this, &SEventSlotCombo::OnGetMenuContent)
						.ButtonContent()
						[
							SNew(SEventSlotChip)
								.OnNavigate(InArgs._OnNavigate)
								.OnMenu(InArgs._OnMenu)
								.ShowClearButton(this, &SEventSlotCombo::ShowClearButton)
								.EnableNavigation(InArgs._EnableNavigation)
								.Text(this, &SEventSlotCombo::GetText)
								.ToolTipText(this, &SEventSlotCombo::GetToolTipText)
								.IsSelected(this, &SEventSlotCombo::IsSelected)
								.OnClearPressed(this, &SEventSlotCombo::OnClearPressed)
								.OnEditPressed(this, &SEventSlotCombo::OnEditTag)
								.OnMenu(this, &SEventSlotCombo::OnTagMenu)
						]
				]
		];
}

bool SEventSlotCombo::IsValueEnabled() const
{
	if (PropertyHandle.IsValid())
	{
		return !PropertyHandle->IsEditConst();
	}

	return !bIsReadOnly;
}

void SEventSlotCombo::OnMenuOpenChanged(const bool bOpen) const
{
	if (bOpen && SlotPicker.IsValid())
	{
		//const FGameplayTag TagToHilight = GetCommonTag();
		//TagPicker->RequestScrollToView(TagToHilight);

		//ComboButton->SetMenuContentWidgetToFocus(TagPicker->GetWidgetToFocusOnOpen());
	}
}

TSharedRef<SWidget> SEventSlotCombo::OnGetMenuContent()
{
	SlotPicker = SNew(SEventSlotPicker)
		.PropertyHandle(this->PropertyHandle)
		.Filter(this->Filter)
		.OnSlotChanged_Raw(this, &SEventSlotCombo::OnSlotUpdated);
		

	return SlotPicker.ToSharedRef();
}

bool SEventSlotCombo::ShowClearButton() const
{
	if (this->PropertyHandle.IsValid())
	{
		const auto StructProp = CastField<FStructProperty>(this->PropertyHandle->GetProperty());
		if (StructProp && StructProp->Struct->IsChildOf(FEventSlot::StaticStruct()))
		{
			bool IsNone = true;

			PropertyHandle->EnumerateRawData([&IsNone](void* RawData, const int32, const int32)
				{
					auto Value = static_cast<FEventSlot*>(RawData);
					IsNone = Value->IsNone();
					return IsNone;
				});
			
			return !IsNone;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

void SEventSlotCombo::OnSlotUpdated(FEventSlot Slot)
{
	this->InlineSlot = Slot;
	this->OnSlotChanged.ExecuteIfBound(Slot);
}

FText SEventSlotCombo::GetText() const
{
	FName EventName;

	if (this->PropertyHandle.IsValid())
	{
		const auto StructProp = CastField<FStructProperty>(this->PropertyHandle->GetProperty());
		if (StructProp && StructProp->Struct->IsChildOf(FEventSlot::StaticStruct()))
		{
			PropertyHandle->EnumerateRawData([&EventName](void* RawData, const int32, const int32)
				{
					auto Value = static_cast<FEventSlot*>(RawData);
					EventName = *Value;
					return true;
				});

			
			this->OnSlotChanged.ExecuteIfBound(EventName);
		}
	}
	else
	{
		EventName = this->InlineSlot;
	}
	
	return FText::FromName(EventName);
}

FText SEventSlotCombo::GetToolTipText() const
{
	return FText::FromString("Select an Event");
}

bool SEventSlotCombo::IsSelected() const
{
	return false;
}

FReply SEventSlotCombo::OnClearPressed()
{
	this->OnClearTag();
	return FReply::Handled();
}

void SEventSlotCombo::OnClearTag()
{
	if (this->PropertyHandle.IsValid())
	{
		PropertyHandle->SetValueFromFormattedString(
			FString::Printf(TEXT("(EventName=\"%s\")"), ""));
	}
}

FReply SEventSlotCombo::OnEditTag() const
{
	FReply Reply = FReply::Handled();

	if (ComboButton->ShouldOpenDueToClick())
	{
		ComboButton->SetIsOpen(true);
	}
	else
	{
		ComboButton->SetIsOpen(false);
	}

	return Reply;
}

FReply SEventSlotCombo::OnTagMenu(const FPointerEvent& MouseEvent)
{
	return FReply::Handled();
}