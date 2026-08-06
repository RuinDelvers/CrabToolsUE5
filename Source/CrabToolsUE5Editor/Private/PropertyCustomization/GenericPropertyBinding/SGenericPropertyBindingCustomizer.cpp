#include "PropertyCustomization/GenericPropertyBinding/SGenericPropertyBindingCustomizer.h"
#include "PropertyCustomization/GenericPropertyBinding/SGenericPropertyPath.h"
#include "PropertyCustomization/GenericPropertyBinding/SGenericPropertySelector.h"
#include "Properties/GenericPropertyBinding.h"
#include "EdGraphSchema_K2.h"
#include "Kismet2/BlueprintEditorUtils.h"

#define LOCTEXT_NAMESPACE "SGenericPropertyBindingCustomizer"

SGenericPropertyCustomizer::SGenericPropertyCustomizer()
{

}

void SGenericPropertyCustomizer::Construct(const FArguments& InArgs, TSharedPtr<IPropertyHandle> InitProp)
{
	this->PropertyHandle = InitProp;
	auto PinType = this->GetPinTypeForProp();
	const UEdGraphSchema_K2* Schema = GetDefault<UEdGraphSchema_K2>();

	ChildSlot
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()[
			SAssignNew(ComboButton, SComboButton)
				.HasDownArrow(true)
				.ContentPadding(1)
				.OnGetMenuContent(this, &SGenericPropertyCustomizer::OnGetMenuContent)
				.ToolTipText(this, &SGenericPropertyCustomizer::GetPathText)
				.ButtonContent()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
					.VAlign(VAlign_Center)
					.HAlign(HAlign_Center)
				[
					SAssignNew(ComboButtonText, STextBlock)
						.Text(this->GetDisplayText())
				]
				+ SHorizontalBox::Slot()
					.AutoWidth()
					.VAlign(VAlign_Center)
					.Padding(1.0f, 0.0f)
				[
					SNew(SImage)
						.ColorAndOpacity(Schema->GetPinTypeColor(PinType))
						.Image(FBlueprintEditorUtils::GetIconFromPin(PinType, true))
				]
				
			]
		]
		+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Fill)
			.Padding(0.0f, 0.0f)
		[
			SNew(SButton)
				.ToolTipText(LOCTEXT("ClearButtonDescription", "Clear Binding"))
				.VAlign(VAlign_Center)
				.ContentPadding(FMargin(0, 0))
				//.Visibility(this, &SGenericPropertyCustomizer::GetClearVisibility)
				.OnClicked(this, &SGenericPropertyCustomizer::ClearValue)
				.Content()
				[
					SNew(SImage)
						.Image(FAppStyle().GetBrush("PropertyWindow.Button_Clear"))
				]
		]
	];

	InitProp->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(this, &SGenericPropertyCustomizer::OnPropertyValueChanged));
}

FEdGraphPinType SGenericPropertyCustomizer::GetPinTypeForProp() const
{
	FEdGraphPinType PinType;
	auto Field = CastField<FStructProperty>(PropertyHandle->GetProperty());
	check(Field);
	
	if (Field->Struct == FGenericObjectPropertyBinding::StaticStruct())
	{
		PinType.PinCategory = UEdGraphSchema_K2::PC_Object;
	}
	else if (Field->Struct == FGenericBoolPropertyBinding::StaticStruct())
	{
		PinType.PinCategory = UEdGraphSchema_K2::PC_Boolean;
	}

	return PinType;
}

TSharedRef<SWidget> SGenericPropertyCustomizer::OnGetMenuContent()
{
	return SNew(SScrollBox)
		+ SScrollBox::Slot()
		[
			SNew(SGenericPropertySelector, PropertyHandle)
				.OnClosed(SGenericPropertySelector::FClosed::CreateSP(this, &SGenericPropertyCustomizer::OnSelectorClosed))
				
		];
}

void SGenericPropertyCustomizer::OnPropertyValueChanged()
{
	this->ComboButtonText->SetText(this->GetDisplayText());
}

void SGenericPropertyCustomizer::OnSelectorClosed()
{
	this->ComboButton->SetIsOpen(false);
}

FReply SGenericPropertyCustomizer::ClearValue()
{
	auto Prop = CastFieldChecked<FStructProperty>(this->PropertyHandle->GetProperty());

	if (Prop->Struct == FGenericObjectPropertyBinding::StaticStruct())
	{
		PropertyHandle->EnumerateRawData([](void* RawData, const int32, const int32)
			{
				*static_cast<FGenericObjectPropertyBinding*>(RawData) = FGenericObjectPropertyBinding();
				return true;
			});
	}
	else if (Prop->Struct == FGenericBoolPropertyBinding::StaticStruct())
	{
		PropertyHandle->EnumerateRawData([](void* RawData, const int32, const int32)
			{
				*static_cast<FGenericBoolPropertyBinding*>(RawData) = FGenericBoolPropertyBinding();
				return true;
			});
	}
	//PropertyHandle->SetValueFromFormattedString("()");
	this->ComboButtonText->SetText(this->GetDisplayText());

	return FReply::Handled();
}

FText SGenericPropertyCustomizer::GetPathText() const
{
	FText Result = FText::FromString("None");

	PropertyHandle->EnumerateRawData([this, &Result](void* RawData, const int32, const int32)
		{
			Result = FText::FromString((static_cast<FGenericObjectPropertyBinding*>(RawData))->GetPathString());
			return true;
		});

	return Result;
}

FText SGenericPropertyCustomizer::GetDisplayText() const
{
	FText Result = FText::FromString("None");

	PropertyHandle->EnumerateRawData([this, &Result](void* RawData, const int32, const int32)
		{
			Result = FText::FromString((static_cast<FGenericObjectPropertyBinding*>(RawData))->GetDisplayString());
			return true;
		});

	return Result;
}

/*
EVisibility SGenericPropertyCustomizer::GetClearVisibility() const
{
	this->PropertyHandle->
}
*/

#undef LOCTEXT_NAMESPACE