#include "Properties/GenericPropertyBlueprintLibrary.h"

bool UGenericPropertyBlueprintLibrary::GetBool(const UObject* Source, const FGenericBoolPropertyBinding& Binding)
{
	return Binding.Get(Source);
}

bool UGenericPropertyBlueprintLibrary::BranchtBool(const UObject* Source, const FGenericBoolPropertyBinding& Binding)
{
	return Binding.Get(Source);
}
