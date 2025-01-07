#include "StateMachine/StateMachineBlueprintGeneratedClass.h"
#include "StateMachine/Utils.h"
#include "StateMachine/StateMachineInterface.h"

UStateMachine* FStateMachineArchetypeData::CreateStateMachine(UStateMachine* Parent, FName NewParentKey) const
{
	if (!this->Archetype) { return nullptr; }

	auto NewMachine = DuplicateObject<UStateMachine>(this->Archetype, Parent, NewParentKey);
	NewMachine->SetParentData(Parent, NewParentKey);

	return NewMachine;
}

void FStateMachineArchetypeData::CleanAndSanitize()
{
	this->Archetype = nullptr;
	this->StateData.Empty();
	this->bInstanceEditable = false;
	this->bIsVariable = false;
	this->bCanOverrideStart = false;
	this->Accessibility = EStateMachineAccessibility::PRIVATE;

}

void FStateMachineArchetypeData::AddStateData(FName StateName, FStateArchetypeData Data)
{
	check(!this->StateData.Contains(StateName));

	this->StateData.Add(StateName, Data);
}

UState* UStateMachineBlueprintGeneratedClass::GetStateData(
	UStateMachine* Outer,
	FName MachineName,
	FName StateName)
{
	FStateMachineArchetypeData* MachineData = nullptr;
	UState* BuiltState = nullptr;

	if (StateName.IsNone())
	{
		return nullptr;
	}

	if (MachineName.IsNone())
	{
		MachineData = &this->Archetype;
	}
	else if (this->SubArchetypes.Contains(MachineName)) 
	{
		MachineData = &this->SubArchetypes[MachineName];
	}

	if (MachineData)
	{
		auto DefaultStateData = MachineData->StateData.Find(StateName);

		if (DefaultStateData)
		{
			bool IsOverride = DefaultStateData->bIsOverride;
			bool IsExtension = DefaultStateData->bIsExtension;

			if (IsOverride && IsExtension)
			{
				if (auto Parent = this->GetParent())
				{
					auto ParentState = Parent->GetStateData(Outer, MachineName, StateName);
					BuiltState = DuplicateObject(DefaultStateData->Archetype, Outer);
					BuiltState->Append(ParentState);
					BuiltState->Append(DefaultStateData->Archetype);
				}
				else
				{
					BuiltState = DuplicateObject(DefaultStateData->Archetype, Outer);
					BuiltState->Append(DefaultStateData->Archetype);
				}
			}
			else if (IsOverride)
			{
				BuiltState = DuplicateObject(DefaultStateData->Archetype, Outer);
				BuiltState->Append(DefaultStateData->Archetype);
			}
			else if (IsExtension)
			{
				if (auto Parent = this->GetParent())
				{
					BuiltState = Parent->GetStateData(Outer, MachineName, StateName);
					BuiltState->Append(DefaultStateData->Archetype);
				}
				else
				{
					BuiltState = DuplicateObject(DefaultStateData->Archetype, Outer);
					BuiltState->Append(DefaultStateData->Archetype);
				}
			}
			else
			{
				BuiltState = DuplicateObject(DefaultStateData->Archetype, Outer);
				BuiltState->Append(DefaultStateData->Archetype);
			}
		}
		// Else Default State is null.
		else
		{
			if (auto Parent = this->GetParent())
			{
				BuiltState = Parent->GetStateData(Outer, MachineName, StateName);
			}
		}
	}
	// Machine was null
	else
	{
		if (auto Parent = this->GetParent())
		{
			BuiltState = Parent->GetStateData(Outer, MachineName, StateName);
		}
	}

	return BuiltState;
}

void UStateMachineBlueprintGeneratedClass::AppendEventEmitters(UStateMachine* SM)
{
	for (const auto& Emit : this->Archetype.GetArchetype()->GetEmitters())
	{
		SM->AddEventEmitter(DuplicateObject(Emit, SM));
	}

	if (auto Parent = this->GetParent())
	{
		Parent->AppendEventEmitters(SM);
	}
}

void UStateMachineBlueprintGeneratedClass::AppendPublicStateNames(TSet<FName>& Names) const
{
	for (auto& Data : this->Archetype.StateData)
	{
		if (StateMachineAccessibility::IsPublic(Data.Value.Access))
		{
			Names.Add(Data.Key);
		}
	}

	if (auto Parent = this->GetParent())
	{
		Parent->AppendPublicStateNames(Names);
	}
}

void UStateMachineBlueprintGeneratedClass::CleanAndSanitize()
{
	this->Interfaces.Empty();
	this->SubArchetypes.Empty();
	this->Archetype.CleanAndSanitize();
	this->EventSet.Empty();
}

FName UStateMachineBlueprintGeneratedClass::GetStartState(FName MachineName) const
{
	if (MachineName.IsNone())
	{
		return Cast<UStateMachine>(this->GetDefaultObject())->StartState;
	}
	else
	{
		if (auto Data = this->SubArchetypes.Find(MachineName))
		{
			return Data->Archetype.Get()->StartState;
		}
		else
		{
			return NAME_None;
		}
	}
}

#if WITH_EDITOR
bool UStateMachineBlueprintGeneratedClass::DoesImplementInterface(UStateMachineInterface* Interface) const
{	
	return this->Interfaces.Contains(Interface);	
}
#endif // WITH_EDITOR	

void UStateMachineBlueprintGeneratedClass::AddStateMachine(FStateMachineArchetypeData Data, FName MachineName)
{
	if (MachineName.IsNone())
	{
		this->Archetype = Data;
	}
	else
	{
		check(!this->SubArchetypes.Contains(MachineName));
		this->SubArchetypes.Add(MachineName, Data);
	}
}

TArray<FString> UStateMachineBlueprintGeneratedClass::GetChildAccessibleSubMachines() const
{
	TArray<FString> Names;

	for (auto& SubMachine : this->SubArchetypes)
	{
		bool Check = SubMachine.Value.Accessibility == EStateMachineAccessibility::PUBLIC
			|| SubMachine.Value.Accessibility == EStateMachineAccessibility::PROTECTED
			|| SubMachine.Value.Accessibility == EStateMachineAccessibility::OVERRIDEABLE;

		if (Check)
		{
			Names.Add(SubMachine.Key.ToString());
		}
	}

	if (auto BPGC = Cast<UStateMachineBlueprintGeneratedClass>(this->GetSuperClass()))
	{
		Names.Append(BPGC->GetChildAccessibleSubMachines());
	}

	return Names;
}

TArray<FString> UStateMachineBlueprintGeneratedClass::GetStateOptions(EStateMachineAccessibility Access) const
{
	TArray<FString> Names;

	for (auto& Data : this->Archetype.StateData)
	{
		if (Data.Value.Access == Access)
		{
			Names.Add(Data.Key.ToString());
		}
	}

	if (auto Parent = this->GetParent())
	{
		Names.Append(Parent->GetStateOptions(Access));
	}

	return Names;
}

TArray<FName> UStateMachineBlueprintGeneratedClass::GetSubMachineOptions() const
{
	TArray<FName> Names;

	for (auto& SubMachine : this->SubArchetypes)
	{
		Names.Add(SubMachine.Key);
	}

	return Names;
}

UStateMachine* UStateMachineBlueprintGeneratedClass::ConstructSubMachine(UStateMachine* Outer, FName Key) const
{
	UStateMachine* Constructed = nullptr;

	if (auto Ptr = this->SubArchetypes.Find(Key))
	{
		if (auto SM = Ptr->Archetype)
		{
			Constructed = DuplicateObject(Ptr->Archetype, Outer);

			if (Ptr->bIsVariable)
			{
				FProperty* Prop = this->FindPropertyByName(Key);

				if (Prop && Prop->GetClass() == FObjectProperty::StaticClass())
				{
					FObjectProperty* ObjProp = (FObjectProperty*)Prop;

					if (ObjProp->PropertyClass == Constructed->GetClass())
					{
						ObjProp->SetValue_InContainer(Outer, Constructed);
					}
				}
			}
		}
	}

	return Constructed;
}

EStateMachineAccessibility UStateMachineBlueprintGeneratedClass::GetSubMachineAccessibility(FName Key) const
{
	if (auto SMPtr = this->SubArchetypes.Find(Key))
	{
		return SMPtr->Accessibility;
	}
	else if (auto Parent = Cast<UStateMachineBlueprintGeneratedClass>(this->GetSuperClass()))
	{
		return Parent->GetSubMachineAccessibility(Key);
	}
	else
	{
		return EStateMachineAccessibility::PRIVATE;
	}
}

void UStateMachineBlueprintGeneratedClass::CollectExtendibleStates(
	TSet<FString>& StateNames,
	FName SubMachineName) const
{
	if (SubMachineName.IsNone())
	{
		for (auto& Data : this->Archetype.StateData)
		{
			if (StateMachineAccessibility::IsExtendible(Data.Value.Access))
			{
				StateNames.Add(Data.Key.ToString());
			}
		}
	}
	else
	{
		if (auto SubM = this->SubArchetypes.Find(SubMachineName))
		{
			for (auto& Data : SubM->StateData)
			{
				if (StateMachineAccessibility::IsExtendible(Data.Value.Access))
				{
					StateNames.Add(Data.Key.ToString());
				}
			}
		}
	}
}

void UStateMachineBlueprintGeneratedClass::CollectOverrideableStates(
	TSet<FString>& StateNames,
	FName SubMachineName) const
{
	if (SubMachineName.IsNone())
	{
		for (auto& Data : this->Archetype.StateData)
		{
			if (StateMachineAccessibility::IsOverrideable(Data.Value.Access))
			{
				StateNames.Add(Data.Key.ToString());
			}
		}
	}
	else
	{
		if (auto SubM = this->SubArchetypes.Find(SubMachineName))
		{
			for (auto& Data : SubM->StateData)
			{
				if (StateMachineAccessibility::IsOverrideable(Data.Value.Access))
				{
					StateNames.Add(Data.Key.ToString());
				}
			}
		}
	}
}

const FStateMachineArchetypeData* UStateMachineBlueprintGeneratedClass::GetMachineArchetypeData(FName MachineName) const
{

	const FStateMachineArchetypeData* Found = nullptr;

	if (MachineName.IsNone())
	{
		Found = &this->Archetype;
	}
	else
	{
		if (auto Machine = this->SubArchetypes.Find(MachineName))
		{
			Found = Machine;
		}
	}

	return Found;
}

const FStateArchetypeData* UStateMachineBlueprintGeneratedClass::GetStateArchetypeData(FName StateName, FName MachineName) const
{
	const FStateArchetypeData* Found = nullptr;

	if (MachineName.IsNone())
	{
		
	}
	else
	{
		if (auto SubM = this->SubArchetypes.Find(MachineName))
		{
			if (auto Data = SubM->StateData.Find(StateName))
			{
				Found = Data;
			}
			else
			{
				if (auto Parent = this->GetParent())
				{
					Found = Parent->GetStateArchetypeData(StateName, MachineName);
				}
			}
		}
	}

	return Found;
}

bool UStateMachineBlueprintGeneratedClass::IsSubMachineNameInUse(FString& Name) const
{
	for (auto& SubGraph : this->SubArchetypes)
	{
		if (SubGraph.Key == Name)
		{
			return true;
		}
	}

	if (auto Parent = this->GetParent())
	{
		return Parent->IsSubMachineNameInUse(Name);
	}
	else
	{
		return false;
	}
}

UStateMachine* UStateMachineBlueprintGeneratedClass::GetMostRecentParentArchetype(FName SubMachineKey) const
{
	if (this->SubArchetypes.Contains(SubMachineKey))
	{
		return this->SubArchetypes[SubMachineKey].Archetype;
	}
	else
	{
		if (auto Parent = this->GetParent())
		{
			return Parent->GetMostRecentParentArchetype(SubMachineKey);
		}
		else
		{
			return nullptr;
		}
	}
}

void UStateMachineBlueprintGeneratedClass::VerifyClass(FNodeVerificationContext& Context)
{

}