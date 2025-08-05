#include "StateMachine/IStateMachineLike.h"

#if WITH_EDITOR
/*
bool FSMPropertySearch::Matches(FProperty* F) const
{
	if (F->HasMetaData("IgnorePropertySearch")) { return false; }

	// If the variable isn't public, then do not expose it yourself.
	if (!(F->GetFlags() & EObjectFlags::RF_Public)) { return false; }

	if (F->GetClass() == this->FClass)
	{
		if (F->GetClass() == FStructProperty::StaticClass())
		{
			FStructProperty* Sf = CastField<FStructProperty>(F);

			if (Sf->Struct == this->Struct)
			{
				return true;
			}
		}
		else if (F->GetClass() == FObjectProperty::StaticClass())
		{
			FObjectProperty* Of = CastField<FObjectProperty>(F);

			if (Of->PropertyClass == this->Class)
			{
				return true;
			}
		}
		else if (auto DProp = CastField<FMulticastInlineDelegateProperty>(F))
		{
			return DProp->SignatureFunction->IsSignatureCompatibleWith(this->FunctionSignature);
		}
		else
		{
			return true;
		}
	}

	return false;
}
*/
#endif //WITH_EDITOR
/*
FSMPropertySearch FSMPropertySearch::ObjectProperty(UClass* Class)
{
	check(Class);

	FSMPropertySearch Params;

	Params.FClass = FObjectProperty::StaticClass();
	Params.Class = Class;

	return Params;
}

FSMPropertySearch FSMPropertySearch::StructProperty(UScriptStruct* Struct)
{
	check(Struct);

	FSMPropertySearch Params;

	Params.FClass = FStructProperty::StaticClass();
	Params.Struct = Struct;

	return Params;
}

FSMPropertySearch FSMPropertySearch::Property(FFieldClass* FieldClass)
{
	check(FieldClass);

	FSMPropertySearch Params;

	Params.FClass = FieldClass;

	return Params;
}

FSMPropertySearch FSMPropertySearch::InlineDelegate(UFunction* Signature)
{
	check(Signature);

	FSMPropertySearch Params;

	Params.FClass = FMulticastInlineDelegateProperty::StaticClass();
	Params.FunctionSignature = Signature;

	return Params;
}
*/

UObject* UStateMachineDataHelpers::FindDataOfType(TSubclassOf<UObject> Type, UObject* Data, bool& Found)
{
	auto Value = FindDataOfType(Type, Data);
	Found = IsValid(Value);

	return Value;
}

UObject* UStateMachineDataHelpers::FindDataOfType(TSubclassOf<UObject> Type, UObject* Data)
{
	if (IsValid(Data))
	{
		if (Data->IsA(Type))
		{
			return Data;
		}
		else if (Data->Implements<UStateMachineDataInterface>())
		{
			if (auto Value = IStateMachineDataInterface::Execute_FindDataOfType(Data, Type))
			{
				return Value;
			}
		}
	}

	return nullptr;
}

void UStateMachineDataHelpers::FindAllDataOfType(TSubclassOf<UObject> Type, UObject* Data, TArray<UObject*>& ReturnValue, bool& Found)
{
	FindAllDataOfType(Type, Data, ReturnValue);
	Found = ReturnValue.Num() > 0;
}

void UStateMachineDataHelpers::FindAllDataOfType(TSubclassOf<UObject> Type, UObject* Data, TArray<UObject*>& ReturnValue)
{
	if (IsValid(Data))
	{
		if (Data->IsA(Type))
		{
			ReturnValue.Add(Data);
		}

		if (Data->Implements<UStateMachineDataInterface>())
		{
			IStateMachineDataInterface::Execute_FindAllDataOfType(Data, Type, ReturnValue);
		}
	}

}

TScriptInterface<UInterface> UStateMachineDataHelpers::FindDataImplementing(TSubclassOf<UInterface> Type, UObject* Data, bool& Found)
{
	if (auto Value = FindDataImplementing(Type, Data))
	{
		Found = true;
		return Value;
	}

	return nullptr;
}

TScriptInterface<UInterface> UStateMachineDataHelpers::FindDataImplementing(TSubclassOf<UInterface> Type, UObject* Data)
{
	if (IsValid(Data))
	{
		if (Data->GetClass()->ImplementsInterface(Type))
		{
			return Data;
		}
		else if (Data->Implements<UStateMachineDataInterface>())
		{
			return IStateMachineDataInterface::Execute_FindDataImplementing(Data, Type);			
		}
	}

	return nullptr;
}

void UStateMachineDataHelpers::FindAllDataImplementing(TSubclassOf<UInterface> Type, UObject* Data, TArray<TScriptInterface<UInterface>>& ReturnValue)
{
	if (IsValid(Data))
	{
		if (Data->IsA(Type))
		{
			ReturnValue.Add(Data);
		}

		if (Data->Implements<UStateMachineDataInterface>())
		{
			IStateMachineDataInterface::Execute_FindAllDataImplementing(Data, Type, ReturnValue);
		}
	}
}

void UStateMachineDataHelpers::FindAllDataImplementing(TSubclassOf<UInterface> Type, UObject* Data, TArray<TScriptInterface<UInterface>>& ReturnValue, bool& Found)
{
	FindAllDataImplementing(Type, Data, ReturnValue);

	Found = ReturnValue.Num() > 0;
}