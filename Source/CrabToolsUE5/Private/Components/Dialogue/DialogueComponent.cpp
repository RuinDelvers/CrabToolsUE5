#include "Components/Dialogue/DialogueComponent.h"

#include "StateMachine/EventListener.h"
#include "StateMachine/Events.h"

void UDialogueStateComponent::AttemptDialogueWithActor(AActor* Actor)
{
	IEventListenerInterface::Execute_EventWithData(this->GetOwner(), Events::Dialogue::REQUEST_CONFIRMED, Actor);
}

bool UDialogueStateComponent::HandShake(UDialogueStateComponent* Conversee)
{
	if (this->Participants.Num() == 0)
	{
		this->Participants.Add(Conversee);
		Conversee->Participants.Add(this);

		this->OnDialogueStarted.Broadcast();
		Conversee->OnDialogueStarted.Broadcast();

		return true;
	}

	return false;
}

void UDialogueStateComponent::FinishDialogue()
{
	TArray<TObjectPtr<UDialogueStateComponent>> OldParts = this->Participants.Array();
	this->Participants.Empty();

	this->FinishDialogueInner();

	for (const auto& Part : OldParts)
	{
		Part->FinishDialogueInner();
	}
}

void UDialogueStateComponent::FinishDialogueInner()
{
	this->Participants.Empty();
	this->OnDialogueFinished.Broadcast();
}

void UDialogueStateComponent::SendMonologue(UMonologueData* Data)
{
	this->OnMonologueSpawned.Broadcast(Data);
	for (const auto& Parts : this->Participants)
	{
		Parts->OnMonologueSpawned.Broadcast(Data);
	}
}

bool UMonologueData::Step()
{
	this->Index += 1;
	
	if (this->Index < this->MonologueText.Num())
	{
		this->OnMonologueUpdate.Broadcast(this);
		return true;
	}
	else
	{
		this->OnMonologueFinished.Broadcast(this);
		return false;
	}
}

FText UMonologueData::CurrentText() const
{
	if (this->Index >= 0)
	{
		return this->MonologueText[this->Index].Text;
	}
	else
	{
		return FText();
	}
}

void UMonologueData::Reset()
{
	this->Index = -1;
}

bool UMonologueData::Finished() const
{
	int Num = this->MonologueText.Num();
	return Num == 0 || this->Index >= Num;
}

void UMonologueData::ClearListeners(UObject* Obj)
{
	this->OnMonologueUpdate.RemoveAll(Obj);
	this->OnMonologueFinished.RemoveAll(Obj);
}