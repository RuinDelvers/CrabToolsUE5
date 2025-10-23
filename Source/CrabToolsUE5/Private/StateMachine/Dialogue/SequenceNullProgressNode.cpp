#include "StateMachine/Dialogue/SequenceNullProgressNode.h"
#include "LevelSequenceActor.h"
#include "StateMachine/Events.h"

USequenceNullProgressNode::USequenceNullProgressNode()
{
	this->AddEmittedEvent(Events::Dialogue::NULL_SEQUENCE_PROGRESS);
}

void USequenceNullProgressNode::Enter_Inner_Implementation()
{
	if (auto Actor = Cast<ALevelSequenceActor>(this->GetOwner()))
	{
		if (!IsValid(Actor->GetSequence()))
		{
			this->EmitEvent(Events::Dialogue::NULL_SEQUENCE_PROGRESS);
		}
	}
}