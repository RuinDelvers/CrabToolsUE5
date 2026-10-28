#include "StateMachine/AI/StopMovementNode.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "StateMachine/Events.h"
#include "StateMachine/AI/AIStructs.h"
#include "AI/MovementRequest.h"
#include "StateMachine/Logging.h"
#include "NavigationPath.h"

void UStopMovementNode::Enter_Inner_Implementation()
{
	if (this->bOnEnter)
	{
		this->GetAIController()->StopMovement();
	}
}

void UStopMovementNode::Exit_Inner_Implementation()
{
	if (this->bOnExit)
	{
		this->GetAIController()->StopMovement();
	}
}
