#include "StateMachine/Dialogue/MonologueNode.h"
#include "Components/Dialogue/DialogueComponent.h"
#include "StateMachine/Events.h"


UMonologueNode::UMonologueNode()
{
	this->Data = CreateDefaultSubobject<UMonologueData>(TEXT("MonologueData"));
}

void UMonologueNode::Initialize_Inner_Implementation()
{
	Super::Initialize_Inner_Implementation();
	
	this->Data->OnMonologueFinished.AddDynamic(this, &UMonologueNode::HandleFinish);
}

void UMonologueNode::Enter_Inner_Implementation()
{
	if (this->Data->Finished())
	{
		this->EmitEvent(Events::Dialogue::MONOLOGUE_FINISHED);
	}
	else
	{		
		this->GetDialogueComponent()->SendMonologue(this->Data);
		this->Data->Step();
	}
}

void UMonologueNode::Exit_Inner_Implementation()
{
	this->Data->Reset();

	if (this->bNullOnExit)
	{
		this->GetDialogueComponent()->NullDialogue();
	}
}

void UMonologueNode::GetEmittedEvents(TSet<FName>& Events) const
{
	Events.Add(Events::Dialogue::MONOLOGUE_FINISHED);
}

void UMonologueNode::HandleFinish(UMonologueData* MonologueData)
{
	this->EmitEvent(Events::Dialogue::MONOLOGUE_FINISHED);
}