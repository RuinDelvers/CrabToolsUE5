#include "StateMachine/Dialogue/MonologueNode.h"
#include "Components/Dialogue/DialogueComponent.h"
#include "StateMachine/Events.h"
#include "LevelSequenceActor.h"

UMonologueNode::UMonologueNode()
{
	this->AddEmittedEvent(Events::Dialogue::MONOLOGUE_FINISHED);
	this->Data = CreateDefaultSubobject<UMonologueData>(TEXT("MonologueData"));
}

void UMonologueNode::Initialize_Inner_Implementation()
{
	Super::Initialize_Inner_Implementation();

	if (auto Actor = Cast<ALevelSequenceActor>(this->GetOwner()))
	{
		this->Data->SetPlayer(Actor->GetSequencePlayer());
	}
	
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

void UMonologueNode::HandleFinish(UMonologueData* MonologueData)
{
	this->EmitEvent(Events::Dialogue::MONOLOGUE_FINISHED);
}