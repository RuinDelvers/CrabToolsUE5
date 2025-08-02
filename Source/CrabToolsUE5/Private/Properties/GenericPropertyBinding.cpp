#include "Properties/GenericPropertyBinding.h"

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
	else if (this->RootObject)
	{
		BaseClass = this->RootObject;
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
	else if (this->RootObject)
	{
		return this->RootObject;
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

void UGenericPropertyBinding::ClearCache()
{
	this->bCachedValues = false;

	if (this->Child)
	{
		this->Child->ClearCache();
	}
}

void UGenericPropertyBinding::SetCachedProperty() const
{
	if (this->CachedProperty == nullptr)
	{
		this->CachedProperty = this->GetSearchClass()->FindPropertyByName(this->PropertyName);
	}
}

UObject* UGenericPropertyBinding::GetObject() const
{
	return this->GetObject_Local(this->GetOuter());
}

UObject* UGenericPropertyBinding::GetObject_Local(UObject* SourceObject, bool bRecurse) const
{
	UE_LOG(LogTemp, Warning, TEXT("Source Object = %s"), *SourceObject->GetName());

	if (this->bCachedValues)
	{
		if (this->Child && bRecurse)
		{
			if (this->CachedValues.Object.IsValid())
			{
				return this->Child->GetObject_Local(this->CachedValues.Object.Get());
			}
		}
		else
		{
			if (this->CachedValues.Object.IsValid())
			{
				return this->CachedValues.Object.Get();
			}
		}
	}

	this->SetCachedProperty();

	if (this->IsFunctionProperty() && this->IsFunctionReturnObjectProperty())
	{
		this->CachedFunctionCall.BindUFunction(SourceObject, this->PropertyName);
		return this->CachedFunctionCall.Execute();
	}
	else if (auto ObjProp = CastField<FObjectProperty>(this->CachedProperty))
	{
		TObjectPtr<UObject> Output = *ObjProp->ContainerPtrToValuePtr<UObject*>(SourceObject);

		if (this->bCacheData)
		{
			this->CachedValues.Object = Output;
			this->bCachedValues = true;
		}

		if (this->Child && bRecurse)
		{
			this->Child->GetObject_Local(Output);
		}
		else
		{
			return Output;
		}
	}

	return nullptr;
}

bool UGenericPropertyBinding::GetBool() const
{
	return this->GetBool_Local(this->GetOuter());
}

bool UGenericPropertyBinding::GetBool_Local(UObject* SourceObject) const
{
	if (this->Child)
	{
		return this->Child->GetBool_Local(this->GetObject_Local(SourceObject, false));
	}
	else
	{
		if (this->IsFunctionProperty())
		{
			this->CachedBoolCall.BindUFunction(SourceObject, this->PropertyName);
			return this->CachedBoolCall.Execute();
		}
		else
		{
			this->SetCachedProperty();

			if (auto BoolProp = CastField<FBoolProperty>(this->CachedProperty))
			{
				UE_LOG(LogTemp, Warning, TEXT("Checking bool prop for %s"), *SourceObject->GetName())
				bool Output = *BoolProp->ContainerPtrToValuePtr<bool>(SourceObject);

				if (this->bCacheData)
				{
					this->CachedValues.Bool = Output;
					this->bCachedValues = true;
				}

				return Output;
			}
		}

		return false;
	}
}

#if WITH_EDITOR
void UGenericPropertyBinding::PostEditChangeProperty(FPropertyChangedEvent& Event)
{
	Super::PostEditChangeProperty(Event);

	if (this->IsObjectProperty() || this->IsFunctionReturnObjectProperty())
	{
		this->PropertyType = EGenericPropertyType::OBJECT_TYPE;
	}
	else
	{
		this->PropertyType = EGenericPropertyType::UNKNOWN;
	}

	this->bCanHaveChild = this->IsObjectProperty()
		|| (this->IsFunctionProperty() && this->IsFunctionReturnObjectProperty());

	this->bIsRoot = !this->GetOuter()->IsA<UGenericPropertyBinding>();

	if (!this->bCanHaveChild)
	{
		this->Child = nullptr;
	}

	if (this->bIsRoot)
	{
		this->CastAs = nullptr;
	}
	else
	{
		this->RootObject = nullptr;

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
	}
}


TArray<FString> UGenericPropertyBinding::GetPropertyOptions() const
{
	TArray<FString> Names;

	if (auto Parent = Cast<UGenericPropertyBinding>(this->GetOuter()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Getting parent's result class: %s"), *Parent->GetResultClass()->GetName());
		this->IterateFields(Parent->GetResultClass(), Names);
	}
	else if (this->RootObject)
	{
		this->IterateFields(this->RootObject, Names);
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

		if (!(bHasReturnField && bHasNoParams))
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