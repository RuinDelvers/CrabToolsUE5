#include "Sequencer/StateMachineSequenceDirector.h"
#include "StateMachine/EventListener.h"
#include "Components/Dialogue/DialogueComponent.h"
#include "LevelSequencePlayer.h"

void UStateMachineSequenceDirector::PauseSequence()
{
	this->Player->Pause();
}

void UStateMachineSequenceDirector::ResumeSequence()
{
	this->Player->Play();
}

void UStateMachineSequenceDirector::ProgressDialogue()
{
	IEventListenerInterface::Execute_Event(this->GetOuter(), this->ProgressDialogueEvent);
}

void UStateMachineSequenceDirector::ProgressDialogueWithData(UObject* Data)
{
	IEventListenerInterface::Execute_EventWithData(this->GetOuter(), this->ProgressDialogueEvent, Data);
}

void UStateMachineSequenceDirector::ProgressDialogueWithPause()
{
	this->PauseSequence();
	this->ProgressDialogue();
}

void UStateMachineSequenceDirector::ProgressDialogueWithPauseWithdata(UObject* Data)
{
	this->PauseSequence();
	this->ProgressDialogueWithData(Data);
}

void UStateMachineSequenceDirector::MachineEvent(FEventSlot InEvent)
{
	IEventListenerInterface::Execute_Event(this->GetOuter(), InEvent);
}

void UStateMachineSequenceDirector::StepMonologue()
{
	if (this->CurrentMonologue)
	{
		this->CurrentMonologue->Step();
	}
}

void UStateMachineSequenceDirector::HideMonologue()
{
	if (this->CurrentMonologue)
	{
		this->CurrentMonologue->OnMonologueHidden.Broadcast(this->CurrentMonologue);
	}
}

void UStateMachineSequenceDirector::BindDialogueComponent()
{
	if (auto Actor = Cast<AActor>(this->GetOuter()))
	{
		this->Dialogue = Actor->FindComponentByClass<UDialogueStateComponent>();

		this->Dialogue->OnMonologueSpawned.AddDynamic(this, &UStateMachineSequenceDirector::OnMonologueSpawned);
	}
}

void UStateMachineSequenceDirector::OnMonologueSpawned(UMonologueData* Monologue)
{
	this->CurrentMonologue = Monologue;
	Monologue->OnMonologueFinished.AddDynamic(this, &UStateMachineSequenceDirector::OnMonologueFinished);
}

void UStateMachineSequenceDirector::OnMonologueFinished(UMonologueData* Monologue)
{
	this->CurrentMonologue->OnMonologueFinished.RemoveAll(this);
	this->CurrentMonologue = nullptr;
}

void UStateMachineSequenceDirector::LockMonologueToSection()
{
	if (this->CurrentMonologue)
	{
		this->CurrentMonologue->AddLock(this);
	}
}

void UStateMachineSequenceDirector::StepUnlockMonologue()
{
	if (this->CurrentMonologue)
	{
		this->CurrentMonologue->Step(this);
	}
}

void UStateMachineSequenceDirector::UnlockMonologue()
{
	if (this->CurrentMonologue)
	{
		this->CurrentMonologue->RemoveLock(this);
	}
}