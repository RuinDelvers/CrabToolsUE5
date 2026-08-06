#include "PropertyCustomization/GenericPropertyBinding/GenericPropertyBindingCustomization.h"
#include "IDetailChildrenBuilder.h"
#include "PropertyHandle.h"
#include "DetailWidgetRow.h"
#include "DetailLayoutBuilder.h"
#include "PropertyCustomization/GenericPropertyBinding/SGenericPropertyBindingCustomizer.h"
#include "Properties/GenericPropertyBinding.h"


TSharedRef<IPropertyTypeCustomization> FGenericPropertyBindingCustomization::MakeInstance()
{
	return MakeShareable(new FGenericPropertyBindingCustomization());
}

void FGenericPropertyBindingCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	this->CachedPropertyHandle = StructPropertyHandle;

	HeaderRow.NameContent()
		[
			StructPropertyHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		[
			SNew(SGenericPropertyCustomizer, StructPropertyHandle)
		];
}

void FGenericPropertyBindingCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
}
