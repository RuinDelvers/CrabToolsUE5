#include "StateMachine/IStateMachineLike.h"

UObject* UCompositeObjectData::FindDataOfType(TSubclassOf<UObject> Type, UObject* Data, bool& Found)
{
	auto Value = FindDataOfType(Type, Data);
	Found = IsValid(Value);

	return Value;
}

UObject* UCompositeObjectData::FindDataOfType(TSubclassOf<UObject> Type, UObject* Data)
{
	UObject* FoundData = nullptr;

	if (IsValid(Data))
	{
		if (Data->IsA(Type))
		{
			FoundData = Data;
		}
		else if (auto PipedData = Cast<UCompositeObjectData>(Data))
		{
			if (auto LookUpData = PipedData->Objects.Find(Type))
			{
				FoundData = LookUpData->Objects[0];
			}
		}
	}
	
	return FoundData;
}

void UCompositeObjectData::FindAllDataOfType(TSubclassOf<UObject> Type, UObject* Data, TArray<UObject*>& ReturnValue, bool& Found)
{
	FindAllDataOfType(Type, Data, ReturnValue);
	Found = ReturnValue.Num() > 0;
}

void UCompositeObjectData::FindAllDataOfType(TSubclassOf<UObject> Type, UObject* Data, TArray<UObject*>& ReturnValue)
{
	if (IsValid(Data))
	{
		if (Data->IsA(Type))
		{
			ReturnValue.Add(Data);
		}
		else if (auto PipedData = Cast<UCompositeObjectData>(Data))
		{
			if (auto LookUpData = PipedData->Objects.Find(Type))
			{
				ReturnValue.Append(LookUpData->Objects);
			}
		}
	}
}

TScriptInterface<UInterface> UCompositeObjectData::FindDataImplementing(TSubclassOf<UInterface> Type, UObject* Data, bool& Found)
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

TScriptInterface<UInterface> UCompositeObjectData::FindDataImplementing(TSubclassOf<UInterface> Type, UObject* Data)
{
	TScriptInterface<UInterface> FoundData;

	if (IsValid(Data))
	{
		if (Data->GetClass()->ImplementsInterface(Type))
		{
			FoundData = Data;
		}
		else if (auto PipedData = Cast<UCompositeObjectData>(Data))
		{
			if (auto LookUpData = PipedData->Interfaces.Find(Type))
			{
				FoundData = LookUpData->Objects[0];
			}
		}
	}

	return FoundData;
}

void UCompositeObjectData::FindAllDataImplementing(TSubclassOf<UInterface> Type, UObject* Data, TArray<TScriptInterface<UInterface>>& ReturnValue)
{
	if (IsValid(Data))
	{
		if (Data->GetClass()->ImplementsInterface(Type))
		{
			ReturnValue.Add(TScriptInterface<UInterface>(Data));
		}
		else if (auto PipedData = Cast<UCompositeObjectData>(Data))
		{
			if (auto LookUpData = PipedData->Interfaces.Find(Type))
			{
				for (const auto& Obj : LookUpData->Objects)
				{
					ReturnValue.Add(TScriptInterface<UInterface>(Obj));
				}
			}
		}
	}
}

void UCompositeObjectData::FindAllDataImplementing(TSubclassOf<UInterface> Type, UObject* Data, TArray<TScriptInterface<UInterface>>& ReturnValue, bool& Found)
{
	FindAllDataImplementing(Type, Data, ReturnValue);

	Found = ReturnValue.Num() > 0;
}

UObject* UCompositeObjectData::ConcatData(UObject* Data1, UObject* Data2)
{
	if (auto SMData1 = Cast<UCompositeObjectData>(Data1))
	{
		if (auto SMData2 = Cast<UCompositeObjectData>(Data2))
		{
			SMData2->Objects.Append(SMData2->Objects);
			SMData2->Interfaces.Append(SMData2->Interfaces);

			return SMData2;
		}
		else
		{
			SMData1->Add(Data2);
			return SMData1;
		}
	}
	else if (auto SMData2 = Cast<UCompositeObjectData>(Data2))
	{
		SMData2->Add(Data1);
		return SMData2;
	}
	else
	{
		auto ValidData1 = IsValid(Data1);
		auto ValidData2 = IsValid(Data2);
		if (ValidData1 || ValidData2)
		{
			auto NewData = NewObject<UCompositeObjectData>(ValidData1 ? Data1 : Data2);
			NewData->Add(ValidData1 ? Data2 : Data1);

			return NewData;
		}
		else
		{
			return nullptr;
		}
	}
}

UObject* UCompositeObjectData::ConcatDataArray(const TArray<UObject*>& DataValues)
{
	int Num = DataValues.Num();

	if (Num == 0)
	{
		return nullptr;
	}
	else if (Num == 1)
	{
		return DataValues[0];
	}
	else
	{
		auto Root = DataValues[0];

		for (int i = 1; i < Num; ++i)
		{
			Root = UCompositeObjectData::ConcatData(Root, DataValues[i]);
		}

		return Root;
	}
}

void UCompositeObjectData::Add(UObject* NewData)
{
	if (IsValid(NewData))
	{
		if (auto ObjData = this->Objects.Find(NewData->GetClass()))
		{
			ObjData->Objects.Add(NewData);
		}
		else
		{
			this->Objects.Add(NewData->GetClass(), FStateMachinePipedDataValue(NewData));
		}

		for (const auto& IFace : NewData->GetClass()->Interfaces)
		{
			if (auto IFaceValue = this->Interfaces.Find(IFace.Class))
			{
				IFaceValue->Objects.Add(NewData);
			}
			else
			{
				this->Interfaces.Add(IFace.Class, FStateMachinePipedDataValue(NewData));
			}			
		}
	}
}
