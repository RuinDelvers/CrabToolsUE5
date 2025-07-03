#include "StateMachine/Dialogue/MonologueNode.h"
#include "Components/Dialogue/DialogueComponent.h"
#include "StateMachine/Events.h"
#include "Logging/MessageLog.h"


UMonologueNode::UMonologueNode()
{
	this->Data = CreateDefaultSubobject<UMonologueData>(TEXT("MonologueData"));
}

void UMonologueNode::Initialize_Inner_Implementation()
{
	this->DialogueComponent = this->GetActorOwner()->GetComponentByClass<UDialogueStateComponent>();
	
	if (!IsValid(this->DialogueComponent))
	{
		auto ErrorMessage = NSLOCTEXT("UMonologueNode", "MissingDialogueComponent", "Dialogue component not available from actor: {Actor}");
		ErrorMessage = FText::FormatNamed(
			ErrorMessage,
			TEXT("Actor"),
			FText::FromString(this->GetOwner()->GetName()));

		UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMessage.ToString());

		auto Log = FMessageLog("StateMachineErrors");
		Log.Open(EMessageSeverity::Error, true);
		Log.Message(EMessageSeverity::Error, ErrorMessage);
	}

	this->Data->OnMonologueFinished.AddDynamic(this, &UMonologueNode::HandleFinish);
}

void UMonologueNode::PostTransition_Inner_Implementation()
{
	if (this->Data->Finished())
	{
		this->EmitEvent(Events::Dialogue::MONOLOGUE_FINISHED);
	}
	else
	{		
		this->DialogueComponent->SendMonologue(this->Data);
		this->Data->Step();
	}
}

void UMonologueNode::Exit_Inner_Implementation()
{
	this->Data->Reset();
}

void UMonologueNode::GetEmittedEvents(TSet<FName>& Events) const
{
	Events.Add(Events::Dialogue::MONOLOGUE_FINISHED);
}

void UMonologueNode::HandleFinish(UMonologueData* MonologueData)
{
	this->EmitEvent(Events::Dialogue::MONOLOGUE_FINISHED);
}