#include "StateMachine/RPG/RPGStateNode.h"

void URPGStateNode::Initialize_Inner_Implementation()
{
	this->Component = this->GetActorOwner()->FindComponentByClass<URPGComponent>();

	#if WITH_EDITORONLY_DATA
		if (this->bErrorOnMissingComponent)
		{
			ensure(this->Component);
		}
	#endif
}
