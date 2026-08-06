#include "PropertyCustomization/GenericPropertyBinding/SGenericPropertyField.h"
#include "EdGraphSchema_K2.h"
#include "Styling/StarshipCoreStyle.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "PropertyCustomizationHelpers.h"
#include "AssetRegistry/AssetRegistryModule.h"

#define LOCTEXT_NAMESPACE "SGenericPropertyField"



void SGenericPropertyField::Construct(const FArguments& InArgs, TSharedPtr<IPropertyHandle> PropHandle, FProperty* InitProperty)
{
	this->Property = InitProperty;
	this->PropertyHandle = PropHandle;
	this->bSelectable = InArgs._Selectable.IsBound() || InArgs._Selectable.IsSet() ? InArgs._Selectable : false;
	this->OnSelect.Add(InArgs._OnSelect);
	

	FEdGraphPinType PinType;
	const UEdGraphSchema_K2* Schema = GetDefault<UEdGraphSchema_K2>();

	if (Property)
	{
		Schema->ConvertPropertyToPinType(Property, PinType);
		
	}
	else
	{
		PinType.PinCategory = UEdGraphSchema_K2::PC_Object;
	}

	TSharedPtr<SHorizontalBox> Body;

	ChildSlot
		[
			SAssignNew(HighlightBorder, SBorder)
				.BorderBackgroundColor(FLinearColor(1, 1, 1, 0))
				.Padding(FMargin(1, 1))
			[
				SAssignNew(Body, SHorizontalBox)
				+ SHorizontalBox::Slot()
					.AutoWidth()
					.VAlign(VAlign_Center)
					.Padding(1.0f, 0.0f)
				[
					SNew(SImage)
						.Image(FBlueprintEditorUtils::GetIconFromPin(PinType, true))
						.ColorAndOpacity(Schema->GetPinTypeColor(PinType))
				]
				+ SHorizontalBox::Slot()
					.VAlign(VAlign_Center)
					.HAlign(this->bSelectable.Get() ? HAlign_Fill : HAlign_Center)
				[
					this->GetLabel()
				]
			]
		];

	if (this->bSelectable.Get() && InitProperty && InitProperty->IsA<FObjectProperty>())
	{
		auto Slot = Body->AddSlot();
		Slot.AutoWidth();
		Slot.AttachWidget(
			SNew(SClassPropertyEntryBox)
				.ToolTipText(LOCTEXT("CastButtonDescription", "Cast As Type"))
				.AllowedClasses({ this->GetResultClass() })
				.SelectedClass(this->GetResultClass())
				.OnSetClass(this, &SGenericPropertyField::OnClassChosen));
	}
}

FReply SGenericPropertyField::OnMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
	this->bMouseDown = true;
	return FReply::Handled();
}

FReply SGenericPropertyField::OnMouseButtonUp(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
	if (this->bMouseDown)
	{
		this->bMouseDown = false;		
		this->OnSelect.Broadcast(FPropertyChoice(this->Property));
	}

	return FReply::Handled();
}

void SGenericPropertyField::OnMouseEnter(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
	if (this->bSelectable.Get())
	{
		this->HighlightBorder->SetBorderBackgroundColor(FLinearColor(1, 1, 1, 0.8));
	}
}

void SGenericPropertyField::OnMouseLeave(const FPointerEvent& MouseEvent)
{
	if (this->bSelectable.Get())
	{
		this->HighlightBorder->SetBorderBackgroundColor(FLinearColor(1, 1, 1, 0));
	}
}

UClass* SGenericPropertyField::GetResultClass() const
{
	if (auto ObjProp = CastField<FObjectProperty>(this->Property))
	{
		return ObjProp->PropertyClass;
	}
	else if (auto DeleProp = CastField<FDelegateProperty>(this->Property))
	{
		if (auto ReturnProp = CastField<FObjectProperty>(DeleProp->SignatureFunction->GetReturnProperty()))
		{
			return ReturnProp->PropertyClass;
		}
		else
		{
			return nullptr;
		}
	}
	else
	{
		return nullptr;
	}
}

FReply SGenericPropertyField::OnCastClassClicked()
{
	return FReply::Handled();
}

TSharedRef<STextBlock> SGenericPropertyField::GetLabel() const
{
	if (Property)
	{
		return SNew(STextBlock)
			.Text(FText::FromName(Property->GetFName()));
	}
	else
	{
		return SNew(STextBlock)
			.Text(FText::FromName(this->PropertyHandle->GetProperty()->GetOwnerClass()->GetFName()));
	}
}

void SGenericPropertyField::OnClassChosen(const UClass* Chosen)
{
	this->OnSelect.Broadcast(FPropertyChoice(this->Property, Chosen));
}

const UStruct* FPropertyChoice::GetSearchStruct(TSharedPtr<IPropertyHandle> Default) const
{
	if (this->CastClass)
	{
		return this->CastClass;
	}
	else if (this->Property)
	{
		if (auto ObjProp = CastField<FObjectProperty>(this->Property))
		{
			return ObjProp->PropertyClass;
		}
		else if (auto StructProp = CastField<FStructProperty>(this->Property))
		{
			return StructProp->Struct;
		}
		else
		{
			return nullptr;
		}
	}
	else
	{
		return Default->GetProperty()->GetOwnerClass();
	}
}

#undef LOCTEXT_NAMESPACE


