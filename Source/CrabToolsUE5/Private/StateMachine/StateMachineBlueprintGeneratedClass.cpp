#include "StateMachine/StateMachineBlueprintGeneratedClass.h"
#include "StateMachine/Utils.h"
#include "StateMachine/StateMachineInterface.h"
#include "StateMachine/DataStructures.h"

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

	#if WITH_EDITORONLY_DATA
		this->Archetype.EventSet.Empty();
	#endif
}

FName UStateMachineBlueprintGeneratedClass::GetStartState(FName MachineName) const
{
	// Unused, only for starting the call chain.
	bool _CanOverride = false;
	return this->GetStartState_Inner(MachineName, _CanOverride);
}

FName UStateMachineBlueprintGeneratedClass::GetStartState_Local(FName MachineName, bool& CanOverride) const
{
	if (MachineName.IsNone())
	{
		CanOverride = this->Archetype.bCanOverrideStart;
		return this->Archetype.StartState;
	}
	else
	{
		if (auto Data = this->SubArchetypes.Find(MachineName))
		{
			CanOverride = Data->bCanOverrideStart;
			return Data->StartState;
		}
		else
		{
			CanOverride = true;
			return NAME_None;
		}
	}
}

FName UStateMachineBlueprintGeneratedClass::GetStartState_Inner(FName MachineName, bool& CanOverride) const
{
	if (auto Parent = this->GetParent())
	{
		bool ParentAllowedOverride = false;
		auto ParentStart = Parent->GetStartState_Inner(MachineName, ParentAllowedOverride);

		if (ParentAllowedOverride)
		{
			bool LocalOverride = false;
			auto LocalName = this->GetStartState_Local(MachineName, LocalOverride);

			if (LocalName.IsNone())
			{
				return ParentStart;
			}
			else
			{
				return LocalName;
			}
		}
		else
		{
			return ParentStart;
		}
	}
	else
	{
		return this->GetStartState_Local(MachineName, CanOverride);
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
				else
				{
					UE_LOG(LogStateMachine, Error, TEXT("Failed to find to find state data %s for machine %s"),
						*StateName.ToString(),
						*MachineName.ToString());
				}
			}
		}
		else
		{
			UE_LOG(LogStateMachine, Error, TEXT("Failed to find to find state data %s for machine %s"),
				*StateName.ToString(),
				*MachineName.ToString());
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

TSet<FName> UStateMachineBlueprintGeneratedClass::GetEventSet(FName MachineName) const
{
	TSet<FName> EventNames;

	if (MachineName.IsNone())
	{
		EventNames.Append(this->Archetype.EventSet);
	}
	else
	{
		if (this->SubArchetypes.Contains(MachineName))
		{
			EventNames.Append(this->SubArchetypes[MachineName].EventSet);
		}
	}

	if (auto Parent = this->GetParent())
	{
		EventNames.Append(Parent->GetEventSet(MachineName));
	}

	return EventNames;
}

TSet<FName> UStateMachineBlueprintGeneratedClass::GetTotalEventSet() const
{
	TSet<FName> EventNames = this->GetEventSet();

	for (const auto& SubArch : this->SubArchetypes)
	{
		EventNames.Append(this->GetEventSet(SubArch.Key));
	}

	return EventNames;
}

bool UStateMachineBlueprintGeneratedClass::HasEvent(FName InEvent, FName MachineName) const
{
	auto Parent = this->GetParent();

	if (MachineName.IsNone())
	{
		return this->Archetype.EventSet.Contains(InEvent) || (Parent ? Parent->HasEvent(InEvent, MachineName) : false);
	}
	else
	{
		if (auto SubArch = this->SubArchetypes.Find(MachineName))
		{
			return SubArch->EventSet.Contains(InEvent) || (Parent ? Parent->HasEvent(InEvent, MachineName) : false);
		}
	}

	return false;
}

void UStateMachineBlueprintGeneratedClass::GetStateEmittedEvents(FName Machine, FName State, TSet<FName>& OutNames) const
{
	if (auto FoundMachine = Machine.IsNone() ? &this->Archetype : this->SubArchetypes.Find(Machine))
	{
		if (auto FoundState = FoundMachine->StateData.Find(State))
		{
			FoundState->Archetype->GetNode()->GetEmittedEvents(OutNames);
		}
	}

	if (auto Parent = this->GetParent())
	{
		Parent->GetStateEmittedEvents(Machine, State, OutNames);
	}
}