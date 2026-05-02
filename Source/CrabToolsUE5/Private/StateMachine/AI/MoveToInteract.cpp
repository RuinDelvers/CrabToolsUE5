#include "StateMachine/AI/MoveToInteract.h"
#include "Navigation/PathFollowingComponent.h"
#include "StateMachine/Events.h"
#include "Components/Interaction/InteractionSystem.h"
#include "Components/Interaction/InteractableComponent.h"
#include "Components/Interaction/InteractionSystem.h"


UAIMoveToInteractNode::UAIMoveToInteractNode()
{
	this->AddEmittedEvent(Events::AI::CANNOT_INTERACT);
}

void UAIMoveToInteractNode::Initialize_Inner_Implementation()
{
	Super::Initialize_Inner_Implementation();

	this->Component = this->GetActorOwner()->FindComponentByClass<UInteractionComponent>();
}

void UAIMoveToInteractNode::EnterWithData_Inner_Implementation(UObject* Data)
{
	if (auto Interaction = UCompositeObjectData::FindDataOfType<UAIInteractionData>(Data))
	{
		this->InteractWithRequest(Interaction);
	}
	else if (auto Request = UCompositeObjectData::FindDataImplementing<UMovementRequestInterface>(Data).GetObject())
	{		
		switch (IMovementRequestInterface::Execute_GetRequestType(Request))
		{
			case EAIMovementRequestType::NONE: break;
			case EAIMovementRequestType::PAUSE: break;
			case EAIMovementRequestType::RESUME: break;
			default:
				this->UnbindEvents();
				this->CurrentData = nullptr;
		}

		Super::MoveToRequest(Data);
	}
	else
	{
		this->UnbindEvents();
		this->CurrentData = nullptr;
		Super::EnterWithData_Inner_Implementation(Data);
	}
}

void UAIMoveToInteractNode::EventWithData_Inner_Implementation(FName InEvent, UObject* Data, UObject* EventSource)
{
	if (auto Interaction = UCompositeObjectData::FindDataOfType<UAIInteractionData>(Data))
	{
		this->InteractWithRequest(Interaction);
	}
	else if (auto Request = UCompositeObjectData::FindDataImplementing<UMovementRequestInterface>(Data).GetObject())
	{
		switch (IMovementRequestInterface::Execute_GetRequestType(Request))
		{
			case EAIMovementRequestType::NONE: break;
			case EAIMovementRequestType::PAUSE: break;
			case EAIMovementRequestType::RESUME: break;
			default:
				this->UnbindEvents();
				this->CurrentData = nullptr;
		}

		Super::EventWithData_Inner_Implementation(InEvent, Data, EventSource);
	}
	else
	{
		this->UnbindEvents();
		this->CurrentData = nullptr;
		Super::EventWithData_Inner_Implementation(InEvent, Data, EventSource);
	}
}

void UAIMoveToInteractNode::OnMoveCompleted_Implementation(FAIRequestID RequestID, EPathFollowingResult::Type MoveResult)
{
	this->HandleInteraction();
	this->UnbindEvents();
	this->CurrentData = nullptr;

	Super::OnMoveCompleted_Implementation(RequestID, MoveResult);
}

void UAIMoveToInteractNode::Exit_Inner_Implementation()
{
	Super::Exit_Inner_Implementation();
	this->UnbindEvents();
}

bool UAIMoveToInteractNode::HandleInteraction()
{
	bool bDidInteract = false;

	auto InteractComp = this->Component;
	auto Interactable = this->GetInteractable();

	if (InteractComp->HasObject(Interactable))
	{
		if (this->CurrentData->Interaction.IsNone())
		{
			Interactable->InteractDefault(this->Component, this->CurrentData->Data);
		}
		else
		{
			Interactable->Interact(
				this->CurrentData->Interaction,
				this->Component,
				this->CurrentData->Data);
		}

		bDidInteract = true;
	}

	return bDidInteract;
}

bool UAIMoveToInteractNode::HasInteractable() const
{
	if (this->Component)
	{
		return this->Component->HasObject(this->GetInteractable());
	}
	else
	{
		return false;
	}
	
}

void UAIMoveToInteractNode::InteractWithRequest(UAIInteractionData* Interaction)
{
	this->UnbindEvents();
	this->CurrentData = Interaction;

	if (this->HasInteractable())
	{
		if (this->HandleInteraction())
		{
			this->EmitEvent(Events::AI::ARRIVE);
		}
	}
	else if (Interaction->IsValidData())
	{
		this->CurrentData = Interaction;
		this->BindEvents();
		Super::MoveToRequest(Interaction);
	}
	else
	{
		this->EmitEvent(Events::AI::CANNOT_INTERACT);
	}
}

void UAIMoveToInteractNode::RouteRequest(UObject* InData)
{
}

void UAIMoveToInteractNode::BindEvents()
{
	if (this->Component)
	{
		this->Component->OnInteractableAddedEvent.AddUniqueDynamic(
			this,
			&UAIMoveToInteractNode::OnInteractableAdded);
	}
	
}

void UAIMoveToInteractNode::UnbindEvents()
{
	if (this->Component)
	{
		this->Component->OnInteractableAddedEvent.RemoveAll(this);
	}
}

void UAIMoveToInteractNode::OnInteractableAdded(UInteractableComponent* Interactable)
{
	if (Interactable == this->GetInteractable())
	{
		this->StopMovement();
		this->EmitEvent(Events::AI::ARRIVE);
	}
}

UInteractableComponent* UAIMoveToInteractNode::GetInteractable() const
{
	if (this->CurrentData)
	{
		return this->CurrentData->Interactable;
	}
	else
	{
		return nullptr;
	}
}

#if WITH_EDITOR

#endif