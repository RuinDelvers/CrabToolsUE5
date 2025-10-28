#include "StateMachine/Dialogue/DialogueNullNode.h"
#include "Components/Dialogue/DialogueComponent.h"

void UDialogueNullNode::Enter_Inner_Implementation()
{
	if (this->bUseOnEnter)
	{
		this->GetDialogueComponent()->NullDialogue();
	}
}

void UDialogueNullNode::Exit_Inner_Implementation()
{
	if (this->bUseOnExit)
	{
		this->GetDialogueComponent()->NullDialogue();
	}
}