#include "StateMachine/IStateMachineLike.h"

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
	auto Value = FindDataImplementing(Type, Data);
	if (IsValid(Value.GetObject()))
	{
		Found = true;
		return Value;
	}

	Found = false;
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