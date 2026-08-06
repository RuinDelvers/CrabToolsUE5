#include "Properties/GenericPropertyBinding.h"

#define LOCTEXT_NAMESPACE "GenericPropertyBinding"

const FGenericObjectPropertyBindingProperties FGenericObjectPropertyBindingProperties::Default = FGenericObjectPropertyBindingProperties();


void* FGenericObjectPropertyPath::Get(void* Source) const
{
	check(Source);
	FProperty* Prop = SearchType->FindPropertyByName(this->PropertyName);
	void* ReturnObject = nullptr;
	
	if (auto ObjProp = CastField<FObjectProperty>(Prop))
	{
		ReturnObject = *ObjProp->ContainerPtrToValuePtr<UObject*>(Source);
	}
	else if (auto StructProp = CastField<FStructProperty>(Prop))
	{
		ReturnObject = StructProp->ContainerPtrToValuePtr<void>(Source);
	}
	else if (auto Function = SearchType->FindFunction(this->PropertyName))
	{
		if (auto ReturnValue = Function->GetReturnProperty())
		{
			if (ReturnValue->IsA<FObjectProperty>())
			{
				FObjectReturnValue Callback;
				Callback.BindUFunction(reinterpret_cast<UObject*>(Source), this->PropertyName);

				ReturnObject = Callback.Execute();
			}
		}
	}

	return ReturnObject;
}

void FGenericObjectPropertyBinding::PushProperty(FProperty* Prop, UClass* CastClass)
{
	auto SearchType = CastClass == nullptr ? Prop->GetOwnerStruct() : CastClass;

	if (auto ObjProp = CastField<FObjectProperty>(Prop))
	{
		FGenericObjectPropertyPath NewPath;

		NewPath.PropertyName = Prop->GetFName();
		NewPath.SearchType = SearchType;

		this->Path.Add(NewPath);
	}
	else if (auto StructProp = CastField<FStructProperty>(Prop))
	{
		FGenericObjectPropertyPath NewPath;

		NewPath.PropertyName = Prop->GetFName();
		NewPath.SearchType = SearchType;

		this->Path.Add(NewPath);
	}
	else if (auto FunctProp = CastField<FDelegateProperty>(Prop))
	{
		FGenericObjectPropertyPath NewPath;

		NewPath.PropertyName = Prop->GetFName();
		NewPath.SearchType = SearchType;

		this->Path.Add(NewPath);
	}
}

void FGenericObjectPropertyBinding::WriteToProperty(TSharedPtr<IPropertyHandle> Property) const
{
	if (auto StructProp = CastFieldChecked<FStructProperty>(Property->GetProperty()))
	{
		if (StructProp->Struct == FGenericObjectPropertyBinding::StaticStruct())
		{
			FString FormattedString;
			StructProp->Struct->ExportText(FormattedString, this, nullptr, nullptr, PPF_Copy, nullptr);
			Property->SetValueFromFormattedString(FormattedString);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Attempted to write generic property data to invalid property: %s"), *Property->GetProperty()->GetName());
	}
}

UStruct* FGenericObjectPropertyBinding::ResultType() const
{
	if (Path.Num() == 0)
	{
		return nullptr;
	}
	else
	{
		return Path.Last().SearchType;
	}
}

bool FGenericObjectPropertyBinding::ValidProperty(FProperty* CheckProp) const
{
	return CheckProp->GetClass() == FObjectProperty::StaticClass()
		|| CheckProp->GetClass() == FStructProperty::StaticClass();
}

FString FGenericObjectPropertyBinding::GetPathString() const
{
	if (this->Path.Num() == 0)
	{
		return "None";
	}
	else
	{
		FString PathString = this->Path[0].SearchType->GetName();

		for (auto& PathFragment : this->Path)
		{
			PathString.Append(".");
			//PathString.Append(PathFragment.SearchType->GetName() + "::" + PathFragment.PropertyName.ToString());
			PathString.Append(PathFragment.PropertyName.ToString());
		}

		return PathString;
	}
}

FString FGenericObjectPropertyBinding::GetDisplayString() const
{
	if (this->Path.Num() == 0)
	{
		return "None";
	}
	else
	{
		return this->Path.Last().PropertyName.ToString();
	}
}

bool FGenericObjectPropertyBinding::CanFinalize() const
{
	return !this->Path.IsEmpty();
}

void FGenericObjectPropertyBinding::Pop()
{
	if (!this->Path.IsEmpty())
	{
		this->Path.Pop();
	}
}

bool FGenericObjectPropertyBinding::IsEmpty() const
{
	return this->Path.IsEmpty();
}

bool FGenericBoolPropertyBinding::Get(const void* Source) const
{
	return this->Get(const_cast<void*>(Source));
}

bool FGenericBoolPropertyBinding::Get(void* Source) const
{
	check(!this->PropertyName.IsNone());

	void* Container = FGenericObjectPropertyBinding::Get<void>(Source);
	FBoolProperty* Prop = CastFieldChecked<FBoolProperty>(this->RequiredType->FindPropertyByName(PropertyName));

	return *Prop->ContainerPtrToValuePtr<bool>(Source);
}

void FGenericBoolPropertyBinding::Set(void* Source, bool Value)
{
	check(!this->PropertyName.IsNone());
	check(this->Properties.bCanWrite);

	void* Container = FGenericObjectPropertyBinding::Get<void>(Source);
	FBoolProperty* Prop = CastFieldChecked<FBoolProperty>(this->RequiredType->FindPropertyByName(PropertyName));

	Prop->SetPropertyValue_InContainer(Container, Value);
}

bool FGenericBoolPropertyBinding::ValidProperty(FProperty* CheckProp) const
{
	return CheckProp->GetClass() == FBoolProperty::StaticClass();
}

FString FGenericBoolPropertyBinding::GetPathString() const
{
	FString Base = FGenericObjectPropertyBinding::GetPathString();

	Base.Append(".");
	Base.Append(this->PropertyName.ToString());

	return Base;
}

FString FGenericBoolPropertyBinding::GetDisplayString() const
{
	return this->PropertyName.ToString();
}

void FGenericBoolPropertyBinding::PushProperty(FProperty* Prop, UClass* CastClass)
{
	if (auto BoolProp = CastField<FBoolProperty>(Prop))
	{
		this->RequiredType = Prop->GetOwnerStruct();
		this->PropertyName = Prop->GetFName();
	}
	else
	{
		FGenericObjectPropertyBinding::PushProperty(Prop);
	}
}

bool FGenericBoolPropertyBinding::CanFinalize() const
{
	return !this->PropertyName.IsNone();
}

bool FGenericBoolPropertyBinding::IsEmpty() const
{
	return this->PropertyName.IsNone() && FGenericObjectPropertyBinding::IsEmpty();
}

void FGenericBoolPropertyBinding::Pop()
{
	if (this->PropertyName.IsNone())
	{
		FGenericObjectPropertyBinding::Pop();
	}
	else
	{
		this->PropertyName = FName();
	}
}

void FGenericBoolPropertyBinding::WriteToProperty(TSharedPtr<IPropertyHandle> Property) const
{
	if (auto StructProp = CastFieldChecked<FStructProperty>(Property->GetProperty()))
	{
		if (StructProp->Struct == FGenericBoolPropertyBinding::StaticStruct())
		{
			FString FormattedString;
			StructProp->Struct->ExportText(FormattedString, this, nullptr, nullptr, PPF_Copy, nullptr);
			Property->SetValueFromFormattedString(FormattedString);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Attempted to write generic property data to invalid property: %s"), *Property->GetProperty()->GetName());
	}
}

void FGenericFunctionPropertyBinding::Execute(void* Source)
{
	check(!this->PropertyName.IsNone());

	UObject* Obj = this->Get<UObject>(Source);

	if (Obj)
	{
		FObjectFunctionCall Callback;
		Callback.BindUFunction(Obj, this->PropertyName);
	}
}

#undef LOCTEXT_NAMESPACE