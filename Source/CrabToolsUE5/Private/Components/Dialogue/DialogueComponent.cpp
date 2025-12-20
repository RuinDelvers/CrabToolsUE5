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

void UDialogueStateComponent::OnComponentDestroyed(bool bDestroyHierarchy)
{
	Super::OnComponentDestroyed(bDestroyHierarchy);
	this->FinishDialogue();
}

void UDialogueStateComponent::SendDialogue(UDialogueDataStruct* DialogueData)
{
	this->ClearCachedData();
	this->CurrentDialogue = DialogueData;
	this->OnChoicesSpawned.Broadcast(DialogueData);

	for (const auto& Participant : this->Participants)
	{
		if (Participant->OnChoicesSpawned.IsBound())
		{
			Participant->OnChoicesSpawned.Broadcast(DialogueData);
		}
	}
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

bool UDialogueStateComponent::IsInDialogue() const
{
	return this->Participants.Num() > 0;
}

void UDialogueStateComponent::FinishDialogueInner()
{
	this->Participants.Empty();
	this->OnDialogueFinished.Broadcast();
}

void UDialogueStateComponent::SendMonologue(UMonologueData* Data)
{
	this->ClearCachedData();
	this->CurrentMonologue = Data;
	this->OnMonologueSpawned.Broadcast(Data);

	for (const auto& Parts : this->Participants)
	{
		Parts->OnMonologueSpawned.Broadcast(Data);
	}
}

void UDialogueStateComponent::ClearCachedData()
{
	this->CurrentDialogue = nullptr;
	this->CurrentMonologue = nullptr;
}

void UDialogueStateComponent::NullDialogue() const
{
	if (this->Participants.Num() > 0)
	{
		this->OnDialogueNull.Broadcast();

		for (const auto& Part : this->Participants)
		{
			Part->OnDialogueNull.Broadcast();
		}
	}
}

void UMonologueData::Step(UObject* RemovedLock)
{
	if (this->Locks.Remove(RemovedLock) > 0)
	{
		this->OnMonologueLocksUpdated.Broadcast(this);
	}

	if (this->MonologueText.IsEmpty())
	{
		this->OnMonologueFinished.Broadcast(this);
	}
	else if (this->Index < 0)
	{
		this->Index += 1;
		this->StepHelper(false);
	}
	else if (this->Index == this->MonologueText.Num() - 1)
	{
		auto& CurrentCheck = this->Current();

		if (this->Locks.Num() == 0 || !CurrentCheck.bUseLock)
		{
			this->OnMonologueFinished.Broadcast(this);
		}
		else
		{
			this->OnMonologueLocksUpdated.Broadcast(this);
		}
	}
	else
	{
		this->StepHelper(true);
	}
}

void UMonologueData::StepHelper(bool bIncrement)
{
	auto& CurrentCheck = this->Current();

	if (this->Locks.Num() == 0 || !CurrentCheck.bUseLock)
	{
		if (bIncrement)
		{
			this->Index += 1;
		}

		auto& Current = this->Current();

		if (Current.bApplySequenceAction && this->Player)
		{
			USequencerBlueprintHelpers::ApplyAction(this->Player, Current.Action);
		}
		this->OnMonologueUpdate.Broadcast(this);
	}
	else
	{
		this->OnMonologueLocksUpdated.Broadcast(this);
	}
}

void UMonologueData::AddLock(UObject* AddedLock)
{
	this->Locks.Add(AddedLock);
	this->OnMonologueLocksUpdated.Broadcast(this);
}

void UMonologueData::RemoveLock(UObject* AddedLock)
{
	this->Locks.Remove(AddedLock);
	this->OnMonologueLocksUpdated.Broadcast(this);
}

void UMonologueData::SetPlayer(ULevelSequencePlayer* InPlayer)
{
	this->Player = InPlayer;
}

void UMonologueData::Broadcast()
{
	this->OnMonologueUpdate.Broadcast(this);
}

const FMonologueDataStruct& UMonologueData::Current() const
{
	return this->MonologueText[this->Index];
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

bool UMonologueData::IsEmpty() const
{
	return this->Current().Text.IsEmpty();
}

bool UMonologueData::IsLocked() const
{
	return this->Locks.Num() > 0 && this->Current().bUseLock;
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

UObject* UMonologueData::GetDataByClass(UClass* DataClass) const
{
	for (const auto& Data : this->Current().CustomData)
	{
		if (Data && Data->IsA(DataClass))
		{
			return Data;
		}
	}

	return nullptr;
}

void UMonologueData::GetAllDataByClass(UClass* DataClass, TArray<UObject*>& OutData) const
{
	for (const auto& Data : this->Current().CustomData)
	{
		if (Data && Data->IsA(DataClass))
		{
			OutData.Add(Data);
		}
	}
}

#if WITH_EDITOR
void UMonologueData::PostEditChangeProperty(FPropertyChangedEvent& Event)
{
	Super::PostEditChangeProperty(Event);

	for (auto& Data : this->MonologueText)
	{
		if (!Data.bApplySequenceAction)
		{
			Data.Action.Action.Empty();
			Data.Action.TimeStep = FMovieSceneSequencePlaybackParams();
		}
	}
}
#endif