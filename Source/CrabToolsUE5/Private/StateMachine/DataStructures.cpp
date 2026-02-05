#include "StateMachine/DataStructures.h"
#include "StateMachine/StateMachine.h"

void FSubMachineSlot::Initialize(UStateMachine* MachineSource)
{
	switch (this->StateMachineSource)
	{
		case EHierarchyInputType::INLINED:
			if (this->SubMachine)
			{
				this->SubMachine->Initialize(MachineSource->GetOwner());
			}
			break;
		case EHierarchyInputType::DEFINED:
			this->SubMachine = MachineSource->GetSubMachine(*this);
			break;
		case EHierarchyInputType::CUSTOM:
			if (this->MachineClass)
			{
				this->SubMachine = NewObject<UStateMachine>(
					MachineSource,
					this->MachineClass->GetMachineClass(MachineSource).LoadSynchronous());

				if (this->SubMachine)
				{
					this->SubMachine->Initialize(MachineSource->GetOwner());
				}
			}
			break;
	}	
}

bool FSubMachineSlot::DoesReferenceMachine(FName InMachineName) const
{
	switch (this->StateMachineSource)
	{
		case EHierarchyInputType::DEFINED:
			return InMachineName == this->MachineName;
		default: return false;
	}	
}

void FSubMachineSlot::Validate()
{
	switch (this->StateMachineSource)
	{
		case EHierarchyInputType::INLINED:
			this->MachineName = NAME_None;
			break;
		case EHierarchyInputType::DEFINED:
			this->SubMachine = nullptr;
			break;
	}
}