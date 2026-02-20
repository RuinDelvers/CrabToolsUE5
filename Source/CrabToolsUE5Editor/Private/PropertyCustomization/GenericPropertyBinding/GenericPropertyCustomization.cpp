#include "PropertyCustomization/GenericPropertyBinding/GenericPropertyBindingCustomization.h"
#include "IDetailChildrenBuilder.h"
#include "PropertyHandle.h"
#include "DetailWidgetRow.h"
#include "DetailLayoutBuilder.h"
#include "Properties/GenericPropertyBinding.h"


TSharedRef<IDetailCustomization> FGenericPropertyBindingCustomization::MakeInstance()
{
	return MakeShareable(new FGenericPropertyBindingCustomization());
}

void FGenericPropertyBindingCustomization::PendingDelete()
{
}

void FGenericPropertyBindingCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	UE_LOG(LogTemp, Warning, TEXT("CustomDetails (RefOnly) for %s"), *DetailBuilder.GetBaseClass()->GetName());
}

void FGenericPropertyBindingCustomization::CustomizeDetails(const TSharedPtr<IDetailLayoutBuilder>& DetailBuilder)
{
	IDetailCustomization::CustomizeDetails(DetailBuilder);

	UE_LOG(LogTemp, Warning, TEXT("CustomDetails (Shared) for %s"), *DetailBuilder->GetBaseClass()->GetName());

	DetailBuilder->HideCategory("Properties");	

	if (auto BaseClass = DetailBuilder->GetBaseClass())
	{
		for (TFieldIterator<FObjectProperty> FIT(BaseClass, EFieldIteratorFlags::IncludeSuper); FIT; ++FIT)
		{
			auto Prop = *FIT;

			if (Prop->PropertyClass && Prop->PropertyClass->IsChildOf<UGenericPropertyBinding>())
			{
				auto& Cat = Prop->GetMetaData("Category");;
				auto& Category = DetailBuilder->EditCategory(FName(Cat));

				Category.AddCustomRow(FText::FromString("PropertySearchRow"))
					.NameContent()
					[
						SNew(STextBlock)
							.Text(FText::FromString(Prop->GetName()))
					]
					.ValueContent()
					[
						SNew(SButton)
							.Text(FText::FromString("None"))
					];
			}
		}
	}
}
