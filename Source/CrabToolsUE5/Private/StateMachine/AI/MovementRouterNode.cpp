#include "StateMachine/AI/MovementRouterNode.h"
#include "StateMachine/Events.h"

UAIMovementRouterNode::UAIMovementRouterNode()
{
}

void UAIMovementRouterNode::Enter_Inner_Implementation()
{
	if (EnumHasAnyFlags(static_cast<EAIMovementRouterType>(this->FunctionFlags), EAIMovementRouterType::ENTER))
	{
		
	}
}

void UAIMovementRouterNode::EnterWithData_Inner_Implementation(UObject* Data)
{
	if (EnumHasAnyFlags(static_cast<EAIMovementRouterType>(this->FunctionFlags), EAIMovementRouterType::ENTER))
	{
		
	}
}

void UAIMovementRouterNode::Exit_Inner_Implementation()
{
	if (EnumHasAnyFlags(static_cast<EAIMovementRouterType>(this->FunctionFlags), EAIMovementRouterType::EXIT))
	{
		
	}
}

void UAIMovementRouterNode::ExitWithData_Inner_Implementation(UObject* Data)
{
	if (EnumHasAnyFlags(static_cast<EAIMovementRouterType>(this->FunctionFlags), EAIMovementRouterType::EXIT))
	{
		
	}
}
