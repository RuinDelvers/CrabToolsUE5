#include "StateMachine/Dialogue/SequenceOriginSetNode.h"
#include "LevelSequenceActor.h"
#include "DefaultLevelSequenceInstanceData.h"


void USequenceOriginSetNode::Initialize_Inner_Implementation()
{
	if (auto Actor = Cast<ALevelSequenceActor>(this->GetOwner()))
	{
		this->Player = Actor->GetSequencePlayer();
	}
}

void USequenceOriginSetNode::Enter_Inner_Implementation()
{
	if (auto Actor = Cast<ALevelSequenceActor>(this->GetOwner()))
	{
		Actor->bOverrideInstanceData = true;

		if (auto Data = Cast<UDefaultLevelSequenceInstanceData>(Actor->DefaultInstanceData))
		{
			switch (this->Source)
			{
			case ETransformOriginSource::USE_ACTOR_SELF:
				Data->TransformOriginActor = this->GetActorOwner();
				break;
			case ETransformOriginSource::USE_TRANSFORM_SELF:
				Data->TransformOrigin = this->GetActorOwner()->GetActorTransform();
				break;
			case ETransformOriginSource::INLINE:
				Data->TransformOrigin = this->InlineTransform;
				break;
			}
		}
	}
}