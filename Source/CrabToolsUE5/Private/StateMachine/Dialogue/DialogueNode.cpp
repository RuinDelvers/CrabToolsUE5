#include "StateMachine/Dialogue/DialogueNode.h"
#include "StateMachine/Events.h"

void UDialogueNode::Initialize_Inner_Implementation()
{
	for (const auto& Dialogue : this->Choices.DialogueData)
	{
		Dialogue->OnDialogueSelected.AddDynamic(this, &UDialogueNode::HandleDialogueSelection);
	}

	this->DialogueComponent = this->GetActorOwner()->GetComponentByClass<UDialogueStateComponent>();
	check(this->DialogueComponent);
}

void UDialogueNode::GetEmittedEvents(TSet<FName>& Events) const
{
	Super::GetEmittedEvents(Events);

	for (const UDialogueData* Choice : this->Choices.DialogueData)
	{
		Events.Add(Choice->GetEvent());
	}
}

void UDialogueNode::PostTransition_Inner_Implementation()
{
	if (this->DialogueComponent->OnChoicesSpawned.IsBound())
	{
		this->DialogueComponent->OnChoicesSpawned.Broadcast(this->Choices);
	}

	for (const auto& Participant : this->DialogueComponent->Participants)
	{
		if (Participant->OnChoicesSpawned.IsBound())
		{
			Participant->OnChoicesSpawned.Broadcast(this->Choices);
		}
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

void UDialogueNode::HandleDialogueSelection(const UDialogueData* Data)
{
	if (this->Active())
	{
		this->EmitEvent(Data->GetEvent());
	}
}

#pragma region Dialogue State Component

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

#pragma endregion

#pragma region Attempt Dialogue Node
void UAttemptDialogueNode::Initialize_Inner_Implementation()
{
	this->DialogueComponent = this->GetActorOwner()->GetComponentByClass<UDialogueStateComponent>();
	check(this->DialogueComponent);
}

void UAttemptDialogueNode::EventWithData_Inner_Implementation(FName EName, UObject* Data)
{
	UDialogueStateComponent* Comp = Cast<UDialogueStateComponent>(Data);
	
	if (Comp)
	{
		this->HandleComponent(Comp);
	}
	else if (auto Actor = Cast<AActor>(Data))
	{
		Comp = Actor->GetComponentByClass<UDialogueStateComponent>();

		if (Comp)
		{
			this->HandleComponent(Comp);
		}
	}
}

void UAttemptDialogueNode::HandleComponent(UDialogueStateComponent* Comp)
{
	if (Comp->HandShake(this->DialogueComponent))
	{
		this->EmitEvent(Events::Dialogue::DIALOGUE_CONFIRMED);
	}
}

#if WITH_EDITOR
void UAttemptDialogueNode::GetEmittedEvents(TSet<FName>& Events) const
{
	Events.Add(Events::Dialogue::DIALOGUE_CONFIRMED);
}
#endif // WITH_EDITOR
#pragma endregion

void UConfirmDialogueNode::Initialize_Inner_Implementation()
{
	if (auto Comp = this->GetActorOwner()->GetComponentByClass<UDialogueStateComponent>())
	{
		Comp->OnDialogueStarted.AddDynamic(this, &UConfirmDialogueNode::HandleDialogueStarted);
	}
}

void UConfirmDialogueNode::HandleDialogueStarted()
{
	this->EmitEvent(Events::Dialogue::DIALOGUE_CONFIRMED);
}

void UConfirmDialogueNode::GetEmittedEvents(TSet<FName>& Events) const
{
	Events.Add(Events::Dialogue::DIALOGUE_CONFIRMED);
}


void UFinishDialogueNode::Initialize_Inner_Implementation()
{
	this->DialogueComponent = this->GetActorOwner()->GetComponentByClass<UDialogueStateComponent>();
	this->DialogueComponent->OnDialogueFinished.AddDynamic(this, &UFinishDialogueNode::HandleDialogueFinished);
}

void UFinishDialogueNode::Enter_Inner_Implementation()
{
	if (this->bBroadcaster)
	{
		this->DialogueComponent->FinishDialogue();
	}
}

void UFinishDialogueNode::HandleDialogueFinished()
{
	this->EmitEvent(Events::Dialogue::DIALOGUE_FINISHED);
}

#if WITH_EDITOR
void UFinishDialogueNode::GetEmittedEvents(TSet<FName>& Events) const
{
	Events.Add(Events::Dialogue::DIALOGUE_FINISHED);
}
#endif // WITH_EDITOR