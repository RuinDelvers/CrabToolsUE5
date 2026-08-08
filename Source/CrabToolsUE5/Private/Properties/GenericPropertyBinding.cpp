#include "Properties/GenericPropertyBinding.h"

#define LOCTEXT_NAMESPACE "GenericPropertyBinding"

const FGenericObjectPropertyBindingProperties FGenericObjectPropertyBindingProperties::Default = FGenericObjectPropertyBindingProperties();


void* FGenericObjectPropertyPath::Get(void* Source) const
{
	check(Source);
	FProperty* Prop = SearchType->FindPropertyByName(this->PropertyName);
	void* ReturnObject = nullptr;
	
	if (this->bIsFunction)
	{
		if (auto Function = SearchType->FindFunction(this->PropertyName))
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
	}
	else
	{
		if (auto ObjProp = CastField<FObjectProperty>(Prop))
		{
			ReturnObject = *ObjProp->ContainerPtrToValuePtr<UObject*>(Source);
		}
		else if (auto StructProp = CastField<FStructProperty>(Prop))
		{
			ReturnObject = StructProp->ContainerPtrToValuePtr<void>(Source);
		}
		else
		{
			return nullptr;
		}
	}
	
	

	return ReturnObject;
}

void FGenericObjectPropertyBinding::PushProperty(FProperty* Prop)
{
	auto SearchType = Prop->GetOwnerStruct();

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
}

void FGenericObjectPropertyBinding::PushProperty(UFunction* Fn)
{
	if (auto ObjProp = CastField<FObjectProperty>(Fn->GetReturnProperty()))
	{
		FGenericObjectPropertyPath NewPath;

		NewPath.PropertyName = Fn->GetFName();
		NewPath.SearchType = Fn->GetOuterUClass();
		NewPath.bIsFunction = true;

		this->Path.Add(NewPath);
	}
}

void FGenericObjectPropertyBinding::WriteToProperty(TSharedPtr<IPropertyHandle> Property) const
{
	if (auto StructProp = CastFieldChecked<FStructProperty>(Property->GetProperty()))
	{
		FString FormattedString;
		StructProp->Struct->ExportText(FormattedString, this, nullptr, nullptr, PPF_Copy, nullptr);
		Property->SetValueFromFormattedString(FormattedString);
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
	return CheckProp->IsA<FObjectProperty>()
		|| CheckProp->IsA<FStructProperty>();
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

FString FGenericValuePropertyBinding::GetPathString() const
{
	FString Base = FGenericObjectPropertyBinding::GetPathString();

	Base.Append(".");
	Base.Append(this->PropertyName.ToString());

	return Base;
}

FString FGenericValuePropertyBinding::GetDisplayString() const
{
	return this->PropertyName.ToString();
}

void FGenericValuePropertyBinding::PushProperty(FProperty* Prop)
{
	if (this->ValidProperty(Prop))
	{
		this->RequiredType = Prop->GetOwnerStruct();
		this->PropertyName = Prop->GetFName();
		this->bIsFunction = false;
	}
	else
	{
		FGenericObjectPropertyBinding::PushProperty(Prop);
	}
}

void FGenericValuePropertyBinding::PushProperty(UFunction* Fn)
{
	if (this->ValidProperty(Fn->GetReturnProperty()))
	{
		this->PropertyName = Fn->GetFName();
		this->RequiredType = Fn->GetOuterUClass();
		this->bIsFunction = true;
	}
	else
	{
		FGenericObjectPropertyBinding::PushProperty(Fn);
	}
}

bool FGenericValuePropertyBinding::CanFinalize() const
{
	return !this->PropertyName.IsNone();
}

bool FGenericValuePropertyBinding::IsEmpty() const
{
	return this->PropertyName.IsNone() && FGenericObjectPropertyBinding::IsEmpty();
}

void FGenericValuePropertyBinding::Pop()
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

#pragma region Value Types

#pragma region Boolean
bool FGenericBoolPropertyBinding::Get(const void* Source) const
{
	return this->Get(const_cast<void*>(Source));
}

bool FGenericBoolPropertyBinding::Get(void* Source) const
{
	return this->GetHelper<bool>(Source);
}

void FGenericBoolPropertyBinding::Set(void* Source, bool Value)
{
	this->SetHelper<bool>(Source, Value);
}

bool FGenericBoolPropertyBinding::ValidProperty(FProperty* CheckProp) const
{
	return this->ValidPropertyHelper<bool>(CheckProp);
}
#pragma endregion

#pragma region Integer
int FGenericIntPropertyBinding::Get(void* Source) const
{
	return this->GetHelper<int>(Source);
}

int FGenericIntPropertyBinding::Get(const void* Source) const
{
	return this->Get(const_cast<void*>(Source));
}

void FGenericIntPropertyBinding::Set(void* Source, int Value)
{
	this->SetHelper<int>(Source, Value);
}

bool FGenericIntPropertyBinding::ValidProperty(FProperty* CheckProp) const
{
	return this->ValidPropertyHelper<int>(CheckProp);
}

#pragma endregion

#pragma region Float
float FGenericFloatPropertyBinding::Get(void* Source) const
{
	return this->GetHelper<float>(Source);
}

float FGenericFloatPropertyBinding::Get(const void* Source) const
{
	return this->Get(const_cast<void*>(Source));
}

void FGenericFloatPropertyBinding::Set(void* Source, float Value)
{
	this->SetHelper<float>(Source, Value);
}

bool FGenericFloatPropertyBinding::ValidProperty(FProperty* CheckProp) const
{
	return this->ValidPropertyHelper<float>(CheckProp);
}
#pragma endregion

#pragma region FDouble
double FGenericDoublePropertyBinding::Get(void* Source) const
{
	return this->GetHelper<double>(Source);
}

double FGenericDoublePropertyBinding::Get(const void* Source) const
{
	return this->Get(const_cast<void*>(Source));
}

void FGenericDoublePropertyBinding::Set(void* Source, double Value)
{
	this->SetHelper<double>(Source, Value);
}

bool FGenericDoublePropertyBinding::ValidProperty(FProperty* CheckProp) const
{
	return this->ValidPropertyHelper<double>(CheckProp);
}
#pragma endregion
#pragma endregion
#undef LOCTEXT_NAMESPACE