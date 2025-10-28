#include "StateMachine/Dialogue/SequenceWaitUntilPauseNode.h"
#include "LevelSequenceActor.h"
#include "StateMachine/Events.h"
#include "LevelSequencePlayer.h"

USequenceWaitUntilPauseNode::USequenceWaitUntilPauseNode()
{

}

void USequenceWaitUntilPauseNode::Enter_Inner_Implementation()
{
	if (auto Actor = Cast<ALevelSequenceActor>(this->GetActorOwner()))
	{
		Actor->GetSequencePlayer()->OnPause.AddDynamic(this, &USequenceWaitUntilPauseNode::OnPaused);;
	}
}

void USequenceWaitUntilPauseNode::Exit_Inner_Implementation()
{
	Super::Exit_Inner_Implementation();

	if (auto Actor = Cast<ALevelSequenceActor>(this->GetActorOwner()))
	{
		Actor->GetSequencePlayer()->OnPause.RemoveAll(this);
	}
}

void USequenceWaitUntilPauseNode::OnPaused()
{
	this->GetChild()->Enter();
}