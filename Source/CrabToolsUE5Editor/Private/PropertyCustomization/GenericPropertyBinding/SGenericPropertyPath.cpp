#include "PropertyCustomization/GenericPropertyBinding/SGenericPropertyPath.h"
#include "PropertyCustomization/GenericPropertyBinding/SGenericPropertyField.h"

void SGenericPropertyPath::Construct(const FArguments& InArgs, TSharedPtr<IPropertyHandle> PropHandle)
{
	PropertyHandle = PropHandle;

	Reconstruct();
}

void SGenericPropertyPath::Reconstruct()
{
	auto Header = SNew(SGenericPropertyField, PropertyHandle);

	ChildSlot
	[
		SAssignNew(PathContainer, SVerticalBox)
		+ SVerticalBox::Slot()
		.Padding(1.0, 1.0)
		.AutoHeight()
		[
			Header
		]
	];

	for (auto& Prop : this->Path)
	{
		auto Field = SNew(SGenericPropertyField, PropertyHandle, Prop);
		auto Slot = this->PathContainer->AddSlot();

		Slot.AutoHeight();
		Slot.Padding(1.0, 1.0);			
		Slot.AttachWidget(Field);
	}
}

void SGenericPropertyPath::AddProperty(FProperty* NewProp)
{
	this->Path.Add(NewProp);
	this->Reconstruct();
}

void SGenericPropertyPath::Pop()
{
	if (this->Path.Num() > 0)
	{
		PathContainer->RemoveSlot(this->PathContainer->GetSlot(this->PathContainer->NumSlots() - 1).GetWidget());
		this->Path.Pop();
	}
}