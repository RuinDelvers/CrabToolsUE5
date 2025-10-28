#include "StateMachine/Dialogue/SequenceTimeStepNode.h"
#include "LevelSequenceActor.h"

void USequenceTimeStepNode::Initialize_Inner_Implementation()
{
	if (auto Actor = Cast<ALevelSequenceActor>(this->GetOwner()))
	{
		this->Player = Actor->GetSequencePlayer();
	}
}

void USequenceTimeStepNode::Enter_Inner_Implementation()
{
	if (this->Player)
	{
		if (auto Seq = this->Player->GetSequence())
		{
			this->Player->SetPlaybackPosition(this->EnterSteps);
		}
	}
}

void USequenceTimeStepNode::Exit_Inner_Implementation()
{
	if (this->Player)
	{
		if (auto Seq = this->Player->GetSequence())
		{
			this->Player->SetPlaybackPosition(this->ExitSteps);
		}
	}
}