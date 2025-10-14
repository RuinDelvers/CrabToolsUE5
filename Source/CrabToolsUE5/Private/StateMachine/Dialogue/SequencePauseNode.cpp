#include "StateMachine/Dialogue/SequencePauseNode.h"
#include "LevelSequenceActor.h"
#include "StateMachine/Logging.h"



void USequencePauseNode::Initialize_Inner_Implementation()
{
	if (auto Actor = Cast<ALevelSequenceActor>(this->GetOwner()))
	{
		this->Player = Actor->GetSequencePlayer();
	}
	else if (auto CastPlayer = Cast<ULevelSequencePlayer>(this->GetOwner()))
	{
		this->Player = CastPlayer;
	}
	else
	{
		UE_LOG(LogStateMachine, Error, TEXT("Attempted to use Sequence Player node in %s, but no player is given."),
			*this->GetMachine()->GetName());
	}
}

void USequencePauseNode::Enter_Inner_Implementation()
{
	if (this->Player)
	{
		if (this->bPauseOnEnter)
		{
			if (this->bIsPausedOnEnter)
			{
				this->Player->Pause();
			}
			else
			{
				if (!this->Player->IsPlaying())
				{
					this->Player->Play();
				}
			}
		}
	}
}

void USequencePauseNode::Exit_Inner_Implementation()
{
	if (this->Player)
	{
		if (this->bPauseOnExit)
		{
			if (this->bIsPausedOnExit)
			{
				this->Player->Pause();
			}
			else
			{
				if (!this->Player->IsPlaying())
				{
					this->Player->Play();
				}
			}
		}
	}
}