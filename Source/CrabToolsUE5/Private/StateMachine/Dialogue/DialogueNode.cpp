#include "StateMachine/Dialogue/DialogueNode.h"

void UDialogueNode::GetEmittedEvents(TSet<FName>& Events) const
{
	for (const UDialogueData* Choice : this->Choices.DialogueData)
	{
		Events.Add(Choice->GetEvent());
	}
}

void UDialogueNode::PostTransition_Inner_Implementation()
{
	auto Machine = this->GetMachine();

	while (IsValid(Machine))
	{
		IDialogueStateMachine::Execute_BroadcastDialogueChoices(Machine, this->Choices);
		Machine = Machine->GetParentMachine();
	}
}

void UDialogueNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	for (int i = 0; i < this->Choices.DialogueData.Num(); ++i) 
	{
		auto& Choice = this->Choices.DialogueData[i];

		if (!IsValid(Choice))
		{
			this->Choices.DialogueData[i] = NewObject<UDialogueData>(this);
		}
	}
}