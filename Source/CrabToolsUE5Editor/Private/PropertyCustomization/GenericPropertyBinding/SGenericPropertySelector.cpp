#include "PropertyCustomization/GenericPropertyBinding/SGenericPropertySelector.h"
#include "PropertyCustomization/GenericPropertyBinding/SGenericPropertyField.h"
#include "Widgets/Layout/SWidgetSwitcher.h"

void SGenericPropertySelector::Construct(const FArguments& InArgs, TSharedPtr<IPropertyHandle> PropHandle)
{
	PropertyHandle = PropHandle;
	this->InitStructUnion();
	this->OnClosed.Add(InArgs._OnClosed);

	TSharedPtr<SGenericPropertySelectorPage> BasePage;

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(FMargin(2, 1))
		[
			SAssignNew(PathContainer, SGenericPropertyPath, PropertyHandle)
		]
		// Separator between the path and selector pages.
		+ SVerticalBox::Slot()
			.VAlign(VAlign_Center)
			.Padding(1.0, 1.0)
			.AutoHeight()
		[
			SNew(SBorder)
				.BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
				.BorderBackgroundColor(FLinearColor(0.15f, 0.15f, 0.15f, 1.0f))

		]
		+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Center)
			.Padding(FMargin(2, 1))
		[
			SNew(SHorizontalBox)
			// Back button for the page
			+ SHorizontalBox::Slot()
				.AutoWidth()
			[
				SNew(SButton)
					.OnClicked(this, &SGenericPropertySelector::PopPath)
					.ButtonStyle(FAppStyle::Get(), "GraphBreadcrumbButton")
					.IsEnabled(this, &SGenericPropertySelector::CanNavigateBackward)
					//.ToolTipText(NSLOCTEXT("WorkflowNavigationBrowser", "Backward_Tooltip", "Step backward in the tab history. Right click to see full history."))
					[
						SNew(SImage)
							.Image(FAppStyle::GetBrush("GraphBreadcrumb.BrowseBack"))
					]
			]
			// Confirm button, only used for object bindings.
			+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)

			[
				SNew(SButton)
					//.ContentPadding(FMargin(2, 1))
					.IsEnabled(this, &SGenericPropertySelector::IsConfirmable)
					.VAlign(VAlign_Center)
					.Text(FText::FromString("Confirm"))
					.OnClicked(this, &SGenericPropertySelector::OnConfirmClicked)
			]
			// Cancel button to stop searching for a new property.
			+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)

			[
				SNew(SButton)
					//.ContentPadding(FMargin(2, 1))
					.VAlign(VAlign_Center)
					.Text(FText::FromString("Cancel"))
					.OnClicked_Raw(this, &SGenericPropertySelector::OnCancelClicked)
			]
		]
		+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(FMargin(2, 1))
		[
			SAssignNew(SelectorPages, SWidgetSwitcher)
			+SWidgetSwitcher::Slot()
			[
				SAssignNew(BasePage, SGenericPropertySelectorPage, PropHandle, FPropertyChoice(), this->Binding)
					.PropertyChosen(this, &SGenericPropertySelector::OnPropertyChosen_Inner)
			]
		]		
	];
}

bool SGenericPropertySelector::CanNavigateBackward() const
{
	return !this->Binding->IsEmpty();;
}

void SGenericPropertySelector::OnPropertyChosen_Inner(FPropertyChoice Property)
{
	Property.ApplyToBinding(this->Binding);
	// Create a scope so that the Slot is deconstructed and the widget commited to the tree.
	{
		auto NewPage = SNew(SGenericPropertySelectorPage, this->PropertyHandle, Property, Binding)
			.PropertyChosen(this, &SGenericPropertySelector::OnPropertyChosen_Inner);

		auto Slot = this->SelectorPages->AddSlot();
		Slot.AttachWidget(NewPage);
	}

	this->SelectorPages->SetActiveWidgetIndex(this->SelectorPages->GetNumWidgets() - 1);
	this->PathContainer->AddProperty(Property);
}

bool SGenericPropertySelector::IsConfirmable() const
{
	return this->Binding->CanFinalize();
}

void SGenericPropertySelector::InitStructUnion()
{
	auto StructProp = CastFieldChecked<FStructProperty>(PropertyHandle->GetProperty());

	if (StructProp->Struct == FGenericObjectPropertyBinding::StaticStruct())
	{
		this->Binding = MakeShareable<FGenericObjectPropertyBinding>(new FGenericObjectPropertyBinding);
	}
	else if (StructProp->Struct == FGenericBoolPropertyBinding::StaticStruct())
	{
		this->Binding = MakeShareable<FGenericObjectPropertyBinding>(new FGenericBoolPropertyBinding);
	}
}


FReply SGenericPropertySelector::OnCancelClicked() const
{
	this->OnClosed.Broadcast();

	return FReply::Handled();
}

FReply SGenericPropertySelector::OnConfirmClicked() const
{
	this->Binding->WriteToProperty(this->PropertyHandle);
	this->OnClosed.Broadcast();
	return FReply::Handled();
}

FReply SGenericPropertySelector::PopPath()
{
	this->Binding->Pop();
	this->PathContainer->Pop();
	this->SelectorPages->RemoveSlot(this->SelectorPages->GetActiveWidget().ToSharedRef());
	return FReply::Handled();
}


void SGenericPropertySelectorPage::Construct(
	const FArguments& InArgs,
	TSharedPtr<IPropertyHandle> PropHandle,
	FPropertyChoice SearchSource,
	TSharedPtr<FGenericObjectPropertyBinding> InitBinding)
{
	this->Binding = InitBinding;
	this->PropertyHandle = PropHandle;

	auto SearchClass = SearchSource.GetSearchStruct(this->PropertyHandle);
	TSharedPtr<SVerticalBox> Container = SNew(SVerticalBox);

	if (SearchClass)
	{
		for (TFieldIterator<FProperty> PropIt(SearchClass); PropIt; ++PropIt)
		{
			auto Prop = *PropIt;

			if (this->ValidFieldForFinalType(PropHandle, Prop))
			{
				auto& BindingProps = this->Binding->Properties;
				EPropertyFlags SearchFlags = BindingProps.bCanWrite ? CPF_BlueprintVisible : CPF_BlueprintVisible & ~CPF_BlueprintReadOnly;

				if (Prop->HasAnyPropertyFlags(SearchFlags))
				{
					auto Field = SNew(SGenericPropertyField, PropHandle, Prop)
						.OnSelect(InArgs._PropertyChosen)
						.Selectable(true);
					auto Slot = Container->AddSlot();
					Slot.AutoHeight();
					Slot.AttachWidget(Field);
				}
			}
		}

		for (TFieldIterator<UFunction> PropIt(SearchClass); PropIt; ++PropIt)
		{
			auto Fn = *PropIt;

			if (this->ValidFieldForFinalType(PropHandle, Fn->GetReturnProperty()))
			{
				auto Field = SNew(SGenericPropertyField, PropHandle, FPropertyChoice(nullptr, Fn))
					.OnSelect(InArgs._PropertyChosen)
					.Selectable(true);
				auto Slot = Container->AddSlot();
				Slot.AutoHeight();
				Slot.AttachWidget(Field);
			}
		}
	}

	ChildSlot
	[
		Container.ToSharedRef()
	];
}

bool SGenericPropertySelectorPage::ValidFieldForFinalType(TSharedPtr<IPropertyHandle> PropHandle, FProperty* CheckProperty)
{
	return CheckProperty != nullptr &&  CheckProperty != PropHandle->GetProperty() && this->Binding->ValidProperty(CheckProperty);
}
