#include "StateMachine/Dialogue/SequenceStateMachine.h"

TSoftObjectPtr<ULevelSequence> USequenceStateMachine::GetMachineSequence_Implementation() const
{
	if (this->Sequence.IsValid() || this->Sequence.IsPending())
	{
		return this->Sequence;
	}
	else if (this->GetParentMachine() && this->GetParentMachine()->Implements<UCutsceneStateMachine>())
	{
		return ICutsceneStateMachine::Execute_GetMachineSequence(this->GetParentMachine());
	}
	else
	{
		return nullptr;
	}
}

ESequenceStatePlayPhase USequenceStateMachine::GetPlayPhase_Implementation() const
{
	if (this->PlayPhase != ESequenceStatePlayPhase::UNSPECIFIED)
	{
		return this->PlayPhase;
	}
	else if (this->GetParentMachine() && this->GetParentMachine()->Implements<UCutsceneStateMachine>())
	{
		return ICutsceneStateMachine::Execute_GetPlayPhase(this->GetParentMachine());
	}
	else
	{
		return ESequenceStatePlayPhase::UNSPECIFIED;
	}
}