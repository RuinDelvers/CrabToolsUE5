#include "Properties/GenericPropertyBinding.h"

#define LOCTEXT_NAMESPACE "GenericPropertyBinding"

void UGenericPropertyBinding::Initialize()
{
	#if WITH_EDITOR
		this->bWasInitialized = true;
	#endif

	if (this->PropertyName.IsNone())
	{
		return;
	}
	else
	{
		this->SetCachedProperty();

		if (this->Child)
		{
			this->Child->Initialize();
		}

		if (auto Parent = Cast<UGenericPropertyBinding>(this->GetOuter()))
		{
			this->bParentCachesValue = Parent->bCacheData;
		}
		else
		{
			this->bParentCachesValue = true;
		}
	}
}

bool UGenericPropertyBinding::IsContainerType() const
{
	switch (this->PropertyType)
	{
		case EGenericPropertyType::OBJECT_TYPE: return true;
		case EGenericPropertyType::STRUCT_TYPE: return true;
		default: return false;
	}
}

UClass* UGenericPropertyBinding::GetResultClass() const
{
	UClass* BaseClass = nullptr;

	if (this->CastAs)
	{
		return this->CastAs;
	}
	else if (auto Parent = Cast<UGenericPropertyBinding>(this->GetOuter()))
	{
		BaseClass = Parent->GetResultClass();
	}
	else if (this->RootObjectType)
	{
		BaseClass = this->RootObjectType;
	}
	else
	{
		BaseClass = this->GetOuter()->GetClass();
	}

	if (this->IsFunctionReturnObjectProperty())
	{
		auto Func = BaseClass->FindFunctionByName(this->PropertyName);

		if (auto RetVal = CastField<FObjectProperty>(Func->GetReturnProperty()))
		{
			return RetVal->PropertyClass;
		}
		else
		{
			return nullptr;
		}
	}
	else
	{
		if (auto Prop = CastField<FObjectProperty>(BaseClass->FindPropertyByName(this->PropertyName)))
		{
			return Prop->PropertyClass;
		}
		else
		{
			return nullptr;
		}
	}
}

UClass* UGenericPropertyBinding::GetSearchClass() const
{
	if (auto Parent = Cast<UGenericPropertyBinding>(this->GetOuter()))
	{
		return Parent->GetResultClass();
	}
	else if (this->RootObjectType)
	{
		return this->RootObjectType;
	}
	else
	{
		return this->GetOuter()->GetClass();
	}
}

bool UGenericPropertyBinding::IsObjectProperty() const
{
	if (auto Prop = this->GetSearchClass()->FindPropertyByName(this->PropertyName))
	{
		return Prop->IsA<FObjectProperty>();
	}

	return false;
}

bool UGenericPropertyBinding::IsStructProperty() const
{
	if (auto Prop = this->GetSearchClass()->FindPropertyByName(this->PropertyName))
	{
		return Prop->IsA<FStructProperty>();
	}

	return false;
}

bool UGenericPropertyBinding::IsFunctionProperty() const
{
	if (auto Func = this->GetSearchClass()->FindFunctionByName(this->PropertyName))
	{
		return true;
	}

	return false;
}

bool UGenericPropertyBinding::IsFunctionReturnObjectProperty() const
{
	if (auto Func = this->GetSearchClass()->FindFunctionByName(this->PropertyName))
	{
		if (auto ReturnValue = Func->GetReturnProperty())
		{
			if (ReturnValue->IsA<FObjectProperty>())
			{
				return true;
			}
		}
	}

	return false;
}

bool UGenericPropertyBinding::IsDelegateProperty() const
{
	if (auto Prop = this->GetSearchClass()->FindPropertyByName(this->PropertyName))
	{
		return Prop->IsA<FMulticastDelegateProperty>();
	}

	return false;
}

void UGenericPropertyBinding::ClearCache()
{
	this->bCachedValues = false;

	if (this->Child)
	{
		this->Child->ClearCache();
	}
}

UObject* UGenericPropertyBinding::GetRootObject() const
{
	if (auto Parent = Cast<UGenericPropertyBinding>(this->GetOuter()))
	{
		return Parent->GetRootObject();
	}
	else
	{
		return this->GetOuter();
	}
}

UObject* UGenericPropertyBinding::GetSourceObject() const
{
	if (this->RootObjectType)
	{
		if (this->RootObject && this->RootObject->IsA(this->RootObjectType))
		{
			return this->RootObject;
		}
		else
		{
			return this->GetOuter();
		}		
	}
	else
	{
		return this->GetOuter();
	}
}

void UGenericPropertyBinding::SetSource(UObject* NewSource)
{
	if (this->RootObjectType && NewSource && NewSource->IsA(this->RootObjectType))
	{
		this->RootObject = NewSource;
	}
}

void UGenericPropertyBinding::SetCachedProperty()
{
	if (!this->bIsFunctionType)
	{
		auto Prop = this->GetSearchClass()->FindPropertyByName(this->PropertyName);

		if (Prop->IsA<FObjectProperty>())
		{
			this->CachedProperty = CastFieldChecked<FObjectProperty>(Prop);
		}
		else if (Prop->IsA<FBoolProperty>())
		{
			this->CachedProperty = CastFieldChecked<FBoolProperty>(Prop);
		}
		else if (Prop->IsA<FMulticastInlineDelegateProperty>())
		{
			this->CachedProperty = CastFieldChecked<FMulticastInlineDelegateProperty>(Prop);
		}
	}
}

UObject* UGenericPropertyBinding::GetObject() const
{
	#if WITH_EDITOR
		if (!this->bWasInitialized)
		{
			this->NotInitializedError();
			return nullptr;
		}
	#endif
	return (UObject*) this->GetContainer_Local(this->GetSourceObject(), true);
}

void UGenericPropertyBinding::SetObject(UObject* Value)
{
	#if WITH_EDITOR
		if (!this->bWasInitialized)
		{
			this->NotInitializedError();
			return;
		}
	#endif
	this->SetObject_Local(this->GetSourceObject(), Value);
}

void* UGenericPropertyBinding::GetContainer_Local(void* SourceObject, bool bRecurse) const
{
	if (!SourceObject)
	{
		this->NullSourceError();
		return nullptr;
	}

	if (this->bCachedValues)
	{
		void* CachedValue = nullptr;

		switch (this->PropertyType)
		{
			case EGenericPropertyType::STRUCT_TYPE:
				CachedValue = this->GetCachedValue<void*>();
				break;
			case EGenericPropertyType::OBJECT_TYPE:
				CachedValue = this->GetCachedValue<UObject*>();
				break;
			default: break;
		}

		if (this->Child && bRecurse)
		{
			if (this->CachedValues.Object.IsValid())
			{
				return this->Child->GetContainer_Local(CachedValue, true);
			}
		}
		else
		{
			if (CachedValue)
			{
				return CachedValue;
			}
		}
	}

	void* Container = nullptr;

	if (this->bIsFunctionType && this->PropertyType == EGenericPropertyType::OBJECT_TYPE)
	{
		Container = this->BindAndCall<UObject*>(SourceObject);

		UObject* Casted = reinterpret_cast<UObject*>(Container);		
		this->SetCachedValue<UObject*>(Casted);
		return Casted;
	}
	else if (this->PropertyType == EGenericPropertyType::STRUCT_TYPE)
	{
		Container = this->GetProperty<FStructProperty>()->ContainerPtrToValuePtr<void>(SourceObject);
		this->SetCachedValue<void*>(Container);

		if (this->Child && bRecurse)
		{
			return this->Child->GetContainer_Local(Container, true);
		}
		else
		{
			return Container;
		}
	}
	else
	{
		TObjectPtr<UObject> Output = *this->GetProperty<FObjectProperty>()->ContainerPtrToValuePtr<UObject*>(SourceObject);

		this->SetCachedValue<UObject*>(Output);

		if (this->Child && bRecurse)
		{
			return this->Child->GetContainer_Local(Output, true);
		}
		else
		{
			return Output;
		}
	}

	return nullptr;
}

void UGenericPropertyBinding::SetObject_Local(void* SourceObject, UObject* Value)
{
	if (this->Child)
	{
		return this->Child->SetObject_Local(this->GetContainer_Local(SourceObject), Value);
	}
	else
	{
		this->GetProperty<FObjectProperty>()->SetPropertyValue_InContainer(SourceObject, Value);
	}
}

bool UGenericPropertyBinding::GetBool() const
{
	return this->GenericLocalPropGet<bool>(this->GetSourceObject());
}

void UGenericPropertyBinding::SetBool(bool Value)
{
	this->GenericLocalPropSet<bool>(this->GetSourceObject(), Value);
}

int UGenericPropertyBinding::GetInt() const
{
	return this->GenericLocalPropGet<int>(this->GetSourceObject());
}

void UGenericPropertyBinding::SetInt(int Value)
{
	this->GenericLocalPropSet<int>(this->GetSourceObject(), Value);
}

float UGenericPropertyBinding::GetFloat() const
{
	return this->GenericLocalPropGet<float>(this->GetSourceObject());
}

void UGenericPropertyBinding::SetFloat(float Value)
{
	this->GenericLocalPropSet<float>(this->GetSourceObject(), Value);
}

double UGenericPropertyBinding::GetDouble() const
{
	return this->GenericLocalPropGet<double>(this->GetSourceObject());
}

void UGenericPropertyBinding::SetDouble(double Value)
{
	this->GenericLocalPropSet<double>(this->GetSourceObject(), Value);
}

void* UGenericPropertyBinding::GetStruct(bool& bFound) const
{
	void* Container = this->GetContainer_Local(this->GetSourceObject());
	
	bFound = Container != nullptr;
	return Container;
}

void UGenericPropertyBinding::CallFunction()
{
	this->CallFunction_Local(this->GetSourceObject());
}

void UGenericPropertyBinding::CallFunction_Local(void* SourceObject)
{
	if (this->PropertyType == EGenericPropertyType::DELEGATE_TYPE)
	{
		auto& Delegate = CastFieldChecked<FMulticastInlineDelegateProperty>(this->CachedProperty)
			->GetPropertyValue(SourceObject);
		Delegate.ProcessMulticastDelegate<UObject>(nullptr);
	}
	else
	{
		this->CallFunction_Local(this->GetContainer_Local(SourceObject));
	}
}

bool UGenericPropertyBinding::IsBound() const
{
	return !this->PropertyName.IsNone();
}

FString UGenericPropertyBinding::GetPropertyPath() const
{
	FString Base;

	if (auto Parent = Cast<UGenericPropertyBinding>(this->GetOuter()))
	{
		Base = Parent->GetPropertyPath();
	}
	else
	{
		Base = this->GetOuter()->GetName();
	}

	Base.Append("/");
	Base.Append(this->PropertyName.ToString());

	return Base;
}

#if WITH_EDITOR
void UGenericPropertyBinding::PostEditChangeProperty(FPropertyChangedEvent& Event)
{
	Super::PostEditChangeProperty(Event);

	this->ValidateData();
}

void UGenericPropertyBinding::PostLoad()
{
	Super::PostLoad();
}

void UGenericPropertyBinding::ValidateData()
{
	auto Parent = Cast<UGenericPropertyBinding>(this->GetOuter());
	this->bIsFunctionType = this->IsFunctionProperty();

	if (this->IsObjectProperty() || this->IsFunctionReturnObjectProperty())
	{
		this->PropertyType = EGenericPropertyType::OBJECT_TYPE;
	}
	else if (this->IsStructProperty())
	{
		this->PropertyType = EGenericPropertyType::STRUCT_TYPE;
	}
	else if (this->IsDelegateProperty())
	{
		this->PropertyType = EGenericPropertyType::DELEGATE_TYPE;
	}
	else
	{
		this->PropertyType = EGenericPropertyType::UNKNOWN;
	}

	this->bIsRoot = !Parent;
	this->bCanCacheData = !Parent || Parent->bCacheData;

	switch (this->PropertyType)
	{
		case EGenericPropertyType::OBJECT_TYPE:
			this->bCanHaveChild = true;
			break;
		case EGenericPropertyType::STRUCT_TYPE:
			this->bCanHaveChild = true;
			this->bCacheData = this->bCanCacheData;
			break;
		default:
			this->bCanHaveChild = false;
			break;
	}	

	if (!this->bCanCacheData)
	{
		this->bCacheData = false;
	}

	if (!this->bCanHaveChild)
	{
		this->Child = nullptr;
	}

	if (!this->bIsRoot)
	{
		this->RootObjectType = nullptr;
	}

	if (this->CastAs)
	{
		if (auto Result = this->GetResultClass())
		{
			if (!this->CastAs->IsChildOf(this->GetResultClass()))
			{
				this->CastAs = TSubclassOf<UObject>(this->GetResultClass());
			}
		}
	}
	else
	{
		this->CastAs = TSubclassOf<UObject>(this->GetResultClass());
	}

	if (this->Child)
	{
		this->Child->ValidateData();
	}
}


TArray<FString> UGenericPropertyBinding::GetPropertyOptions() const
{
	TArray<FString> Names;

	if (auto Parent = Cast<UGenericPropertyBinding>(this->GetOuter()))
	{
		this->IterateFields(Parent->GetResultClass(), Names);
	}
	else if (this->RootObjectType)
	{
		this->IterateFields(this->RootObjectType, Names);
	}
	else
	{
		this->IterateFields(this->GetOuter()->GetClass(), Names);
	}

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}

void UGenericPropertyBinding::IterateFields(UClass* Class, TArray<FString>& Names) const
{
	for (TFieldIterator<FProperty> Prop(Class, EFieldIteratorFlags::IncludeSuper); Prop; ++Prop)
	{
		FProperty& P = **Prop;

		if (!P.GetBoolMetaData("BlueprintPrivate") && P.HasAnyPropertyFlags(EPropertyFlags::CPF_BlueprintVisible))
		{
			Names.Add(P.GetName());
		}
	}

	this->IterateFunctions(Class, Names);
}

void UGenericPropertyBinding::IterateFunctions(UClass* Class, TArray<FString>& Names) const
{
	auto ValidAccessFlag = this->GetOuter()->GetClass()->IsChildOf(Class) ?
		EFunctionFlags::FUNC_Protected | EFunctionFlags::FUNC_Public : EFunctionFlags::FUNC_Public;

	for (TFieldIterator<UFunction> Prop(Class, EFieldIteratorFlags::IncludeSuper); Prop; ++Prop)
	{
		UFunction& P = **Prop;

		if (P.GetName() == "ExecuteUbergraph")
		{
			continue;
		}

		bool bHasReturnField = false;
		bool bHasNoParams = true;

		for (TFieldIterator<FProperty> It(&P); It; ++It)
		{
			FProperty* FProp = *It;

			if (FProp->HasAnyPropertyFlags(EPropertyFlags::CPF_ReturnParm))
			{
				bHasReturnField = true;
			}
			else
			{
				bHasNoParams = false;
			}
		}

		if (!(bHasReturnField && bHasNoParams) || P.GetReturnProperty()->IsA<FStructProperty>())
		{
			continue;
		}

		if (!P.HasAnyFunctionFlags(EFunctionFlags::FUNC_UbergraphFunction))
		{
			if (!P.HasAnyFunctionFlags(EFunctionFlags::FUNC_Public | EFunctionFlags::FUNC_Protected | EFunctionFlags::FUNC_Private))
			{
				Names.Add(P.GetName());
			}
			else
			{
				if (P.HasAnyFunctionFlags(ValidAccessFlag))
				{
					Names.Add(P.GetName());
				}
			}
		}
	}
}

void UGenericPropertyBinding::PostDuplicate(bool DuplicateForPIE)
{
	Super::PostDuplicate(DuplicateForPIE);

	this->bIsRoot = this->GetOuter()->IsA<UGenericPropertyBinding>();
}

#endif

void UGenericPropertyBinding::NotInitializedError() const
{
	#if WITH_EDITOR
		FMessageLog Log("PIE");
		auto Message = FText::FormatNamed(
			LOCTEXT("NotInitializedFormatter", "Failed to initialize property binding: {Path}"),
			TEXT("Path"), FText::FromString(this->GetPropertyPath()));
		Log.Error(Message);
	#endif
}

void UGenericPropertyBinding::NullSourceError() const
{
	#if WITH_EDITOR
		FMessageLog Log("PIE");
		auto Message = FText::FormatNamed(
			LOCTEXT("NullSourceError", "Null source object attempted: {Path}"),
			TEXT("Path"), FText::FromString(this->GetPropertyPath()));
		Log.Error(Message);
	#endif
}

#undef LOCTEXT_NAMESPACE