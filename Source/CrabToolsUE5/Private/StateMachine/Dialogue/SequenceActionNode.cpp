#include "StateMachine/Dialogue/SequenceActionNode.h"
#include "LevelSequenceActor.h"
#include "StateMachine/Logging.h"



void USequenceActionNode::Initialize_Inner_Implementation()
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

void USequenceActionNode::Enter_Inner_Implementation()
{
	this->ApplyAction(this->EnterAction);
}

void USequenceActionNode::Exit_Inner_Implementation()
{
	this->ApplyAction(this->ExitAction);
}

void USequenceActionNode::ApplyAction(ESequenceActionType Action)
{
	if (this->Player)
	{
		switch (Action)
		{
			case ESequenceActionType::PLAY: this->Player->Play(); break;
			case ESequenceActionType::PAUSE: this->Player->Pause(); break;
			case ESequenceActionType::PLAY_FROM_START:
				this->Player->Stop();
				this->Player->Play();
				break;
			case ESequenceActionType::STOP: this->Player->Stop(); break;
		}
	}
}