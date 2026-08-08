#include "PropertyCustomization/GenericPropertyBinding/SGenericPropertyField.h"
#include "EdGraphSchema_K2.h"
#include "Styling/StarshipCoreStyle.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "PropertyCustomizationHelpers.h"
#include "Properties/GenericPropertyBinding.h"

#define LOCTEXT_NAMESPACE "SGenericPropertyField"



void SGenericPropertyField::Construct(const FArguments& InArgs, TSharedPtr<IPropertyHandle> PropHandle, FPropertyChoice InitChoice)
{
	this->Choice = InitChoice;
	this->PropertyHandle = PropHandle;
	this->bSelectable = InArgs._Selectable.IsBound() || InArgs._Selectable.IsSet() ? InArgs._Selectable : false;
	this->OnSelect.Add(InArgs._OnSelect);
	

	FLinearColor PinColor = FLinearColor::Black;
	const FSlateBrush* Icon = nullptr;
	const UEdGraphSchema_K2* Schema = GetDefault<UEdGraphSchema_K2>();

	if (this->Choice.Property)
	{
		FEdGraphPinType PinType;

		Schema->ConvertPropertyToPinType(Choice.Property, PinType);
		PinColor = Schema->GetPinTypeColor(PinType);
		Icon = FBlueprintEditorUtils::GetIconFromPin(PinType, true);
	}
	else if (this->Choice.Function.IsValid())
	{
		FEdGraphPinType PinType;

		Schema->ConvertPropertyToPinType(this->Choice.Function->GetReturnProperty(), PinType);
		PinColor = Schema->GetPinTypeColor(PinType);
		Icon = FAppStyle::Get().GetBrush("ClassIcon.K2Node_CallFunction");
	}
	else
	{
		FEdGraphPinType PinType;

		PinType.PinCategory = UEdGraphSchema_K2::PC_Object;
		PinColor = Schema->GetPinTypeColor(PinType);
		Icon = FBlueprintEditorUtils::GetIconFromPin(PinType, true);
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
						.Image(Icon)
						.ColorAndOpacity(PinColor)
				]
				+ SHorizontalBox::Slot()
					.VAlign(VAlign_Center)
					.HAlign(this->bSelectable.Get() ? HAlign_Fill : HAlign_Center)
				[
					this->GetLabel()
				]
			]
		];

	if (this->bSelectable.Get() && this->Choice.IsCastableValue())
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
		this->OnSelect.Broadcast(FPropertyChoice(this->Choice.Property, this->Choice.Function.Get()));
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
	if (auto ObjProp = CastField<FObjectProperty>(this->Choice.Property))
	{
		return ObjProp->PropertyClass;
	}
	else if (auto DeleProp = CastField<FDelegateProperty>(this->Choice.Property))
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
	if (Choice.Property)
	{
		return SNew(STextBlock)
			.Text(FText::FromName(this->Choice.Property->GetFName()));
	}
	else if (this->Choice.Function.IsValid())
	{
		return SNew(STextBlock)
			.Text(FText::FromName(this->Choice.Function->GetFName()));
	}
	else
	{
		return SNew(STextBlock)
			.Text(FText::FromName(this->PropertyHandle->GetProperty()->GetOwnerClass()->GetFName()));
	}
}

void SGenericPropertyField::OnClassChosen(const UClass* Chosen)
{
	this->OnSelect.Broadcast(FPropertyChoice(this->Choice.Property, this->Choice.Function.Get(), Chosen));
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
	else if (this->Function.IsValid())
	{
		if (auto ObjProp = CastField<FObjectProperty>(this->Function->GetReturnProperty()))
		{
			return ObjProp->PropertyClass;
		}
		else if (auto StructProp = CastField<FStructProperty>(this->Function->GetReturnProperty()))
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

void FPropertyChoice::ApplyToBinding(TSharedPtr<FGenericObjectPropertyBinding>& Binding) const
{
	if (this->Property)
	{
		Binding->PushProperty(this->Property);
	}
	else if (this->Function.IsValid())
	{
		Binding->PushProperty(this->Function.Get());
	}
}

bool FPropertyChoice::IsCastableValue() const
{
	if (auto ObjProp = CastField<FObjectProperty>(this->Property))
	{
		return true;
	}
	else if (this->Function.IsValid())
	{
		if (auto ReturnProp = CastField<FObjectProperty>(this->Function->GetReturnProperty())) {
			return true;
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

#undef LOCTEXT_NAMESPACE


