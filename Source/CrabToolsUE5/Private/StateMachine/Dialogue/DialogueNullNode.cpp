#include "StateMachine/Dialogue/DialogueNullNode.h"
#include "Components/Dialogue/DialogueComponent.h"

void UDialogueNullNode::Initialize_Inner_Implementation()
{
	this->DialogueComponent = this->GetActorOwner()->GetComponentByClass<UDialogueStateComponent>();
}

void UDialogueNullNode::Enter_Inner_Implementation()
{
	if (this->bUseOnEnter && this->DialogueComponent)
	{
		this->DialogueComponent->OnDialogueNull.Broadcast();
	}
}

void UDialogueNullNode::Exit_Inner_Implementation()
{
	if (this->bUseOnExit && this->DialogueComponent)
	{
		this->DialogueComponent->OnDialogueNull.Broadcast();
	}
}