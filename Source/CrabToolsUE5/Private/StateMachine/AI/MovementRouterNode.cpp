#include "StateMachine/AI/MovementRouterNode.h"
#include "StateMachine/Events.h"
#include "AI/MovementRequest.h"

UAIMovementRouterNode::UAIMovementRouterNode()
{
	this->AddEmittedEvent(Events::AI::ROUTE_REQUEST_ACTOR);
	this->AddEmittedEvent(Events::AI::ROUTE_REQUEST_LOCATION);
	this->AddEmittedEvent(Events::AI::ROUTE_REQUEST_PAUSE);
	this->AddEmittedEvent(Events::AI::ROUTE_REQUEST_RESUME);
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
		if (auto Request = UStateMachinePipedData::FindDataImplementing<UMovementRequestInterface>(Data).GetObject())
		{
			this->HandleRouting(IMovementRequestInterface::Execute_GetRequestType(Request), Request);
		}
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
		if (auto Request = UStateMachinePipedData::FindDataImplementing<UMovementRequestInterface>(Data).GetObject())
		{
			this->HandleRouting(IMovementRequestInterface::Execute_GetRequestType(Request), Request);
		}
	}
}

void UAIMovementRouterNode::HandleRouting(EAIMovementRequestType MovementType, UObject* Request)
{
	switch (MovementType)
	{
		case EAIMovementRequestType::PAUSE: this->EmitEventWithData(Events::AI::ROUTE_REQUEST_PAUSE, Request); break;
		case EAIMovementRequestType::RESUME: this->EmitEventWithData(Events::AI::ROUTE_REQUEST_RESUME, Request); break;
		case EAIMovementRequestType::TO_ACTOR: this->EmitEventWithData(Events::AI::ROUTE_REQUEST_ACTOR, Request); break;
		case EAIMovementRequestType::TO_LOCATION: this->EmitEventWithData(Events::AI::ROUTE_REQUEST_LOCATION, Request); break;
	}
}
