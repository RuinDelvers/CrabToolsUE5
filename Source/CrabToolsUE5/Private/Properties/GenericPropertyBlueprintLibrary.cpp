#include "Properties/GenericPropertyBlueprintLibrary.h"

int UGenericPropertyBlueprintLibrary::GetInt(const UObject* Source, const FGenericIntPropertyBinding& Binding)
{
	return Binding.Get(Source);
}

float UGenericPropertyBlueprintLibrary::GetFloat(const UObject* Source, const FGenericFloatPropertyBinding& Binding)
{
	return Binding.Get(Source);
}

double UGenericPropertyBlueprintLibrary::GetDouble(const UObject* Source, const FGenericDoublePropertyBinding& Binding)
{
	return Binding.Get(Source);
}

bool UGenericPropertyBlueprintLibrary::GetBool(const UObject* Source, const FGenericBoolPropertyBinding& Binding)
{
	return Binding.Get(Source);
}

bool UGenericPropertyBlueprintLibrary::BranchtBool(const UObject* Source, const FGenericBoolPropertyBinding& Binding)
{
	return Binding.Get(Source);
}
