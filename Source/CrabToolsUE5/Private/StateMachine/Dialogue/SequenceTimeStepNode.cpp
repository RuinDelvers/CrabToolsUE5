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
			if (auto Data = this->EnterSteps.Find(Seq))
			{
				this->Player->SetPlaybackPosition(*Data);
			}
		}
	}
}

void USequenceTimeStepNode::Exit_Inner_Implementation()
{
	if (this->Player)
	{
		if (auto Seq = this->Player->GetSequence())
		{
			if (auto Data = this->ExitSteps.Find(Seq))
			{
				this->Player->SetPlaybackPosition(*Data);
			}
		}
	}
}