#include "StateMachine/Dialogue/SequenceSetNode.h"
#include "LevelSequenceActor.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "StateMachine/Events.h"
#include "LevelSequencePlayer.h"
#include "Properties/GenericPropertyBinding.h"

USequenceSetNode::USequenceSetNode()
{
	this->AddEmittedEvent(Events::Dialogue::SEQUENCE_LOADED);
	this->AddEmittedEvent(Events::Dialogue::NULL_SEQUENCE);
}


void USequenceSetNode::Enter_Inner_Implementation()
{
	auto Seq = this->Binding.Get<ULevelSequence>(this);

	if (Seq)
	{
		if (auto Actor = this->GetOwnerAs<ALevelSequenceActor>())
		{
			Actor->SetSequence(Seq);
			this->EmitEvent(Events::Dialogue::SEQUENCE_LOADED);
		}
	}
	else
	{
		this->EmitEvent(Events::Dialogue::NULL_SEQUENCE);
	}
}
