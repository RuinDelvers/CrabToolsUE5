#include "StateMachine/AI/ReturnToNavMeshNode.h"
#include "NavigationSystem.h"
#include "AIController.h"
#include "StateMachine/Events.h"

UReturnToNavMeshNode::UReturnToNavMeshNode()
{
	this->AddEmittedEvent(Events::AI::DONE);
}

void UReturnToNavMeshNode::PostTransition_Inner_Implementation()
{
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(this->GetWorld());

	FVector Point = this->GetActorOwner()->GetActorLocation();
	FNavLocation NavLoc;
	bool bSuccess = NavSys->ProjectPointToNavigation(Point, NavLoc);

	if (bSuccess)
	{
		this->EmitEvent(Events::AI::DONE);
	}
	else
	{
		NavSys->GetRandomPointInNavigableRadius(Point, Radius, NavLoc);
		this->BindCallback();

		this->GetAIController()->MoveToLocation(NavLoc.Location, -1, true, false);		
	}
}

void UReturnToNavMeshNode::OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	UE_LOG(LogTemp, Warning, TEXT("Move finished for ReturnToNavMesh"));
	this->UnbindCallback();
	this->EmitEvent(Events::AI::DONE);
}

void UReturnToNavMeshNode::BindCallback()
{
	auto CtrlQ = this->GetAIController();
	CtrlQ->ReceiveMoveCompleted.AddDynamic(this, &UReturnToNavMeshNode::OnMoveCompleted);
}

void UReturnToNavMeshNode::UnbindCallback()
{
	auto CtrlQ = this->GetAIController();
	CtrlQ->ReceiveMoveCompleted.RemoveAll(this);
}