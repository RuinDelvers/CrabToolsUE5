#include "StateMachine/AI/MoveToInteract.h"
#include "Navigation/PathFollowingComponent.h"
#include "StateMachine/Events.h"
#include "Components/Interaction/InteractionSystem.h"
#include "Components/Interaction/InteractableComponent.h"
#include "Utils/PathFindingUtils.h"


UAIMoveToInteractNode::UAIMoveToInteractNode()
{
	this->AddEmittedEvent(Events::AI::CANNOT_INTERACT);
}

void UAIMoveToInteractNode::Initialize_Inner_Implementation()
{
	Super::Initialize_Inner_Implementation();

	check(IsValid(this->GetInteractionComponent()));
}

void UAIMoveToInteractNode::Enter_Inner_Implementation()
{
	if (this->HasInteractable())
	{
		this->EmitEvent(Events::AI::ARRIVE);
	}
	else
	{
		this->ComputeTarget();
		Super::Enter_Inner_Implementation();
	}
}

void UAIMoveToInteractNode::EnterWithData_Inner_Implementation(UObject* Data)
{
	if (auto Interaction = UStateMachinePipedData::FindDataOfType<UAIInteractionData>(Data))
	{
		if (Interaction->IsValidData())
		{
			this->CurrentData = Interaction;
			this->BindEvents();
		}
		else
		{
			this->CurrentData = nullptr;
		}

		Super::EnterWithData_Inner_Implementation(Interaction->Interactable);
	}
	else
	{
		this->CurrentData = nullptr;
		Super::EnterWithData_Inner_Implementation(Data);
	}
}

void UAIMoveToInteractNode::EventWithData_Inner_Implementation(FName InEvent, UObject* Data, UObject* EventSource)
{
	this->EnterWithData_Inner(Data);
}

void UAIMoveToInteractNode::Exit_Inner_Implementation()
{
	Super::Exit_Inner_Implementation();

	if (this->GetMovementResult() == EPathFollowingResult::Success)
	{
		this->HandleInteraction();
	}
	else if (this->GetMovementResult() == EPathFollowingResult::Aborted)
	{
		this->HandleInteraction();
	}

	if (!this->bCacheInteractionData)
	{
		this->CurrentData = nullptr;
	}

	if (auto Component = this->GetInteractionComponent())
	{
		Component->OnInteractableAddedEvent.RemoveAll(this);
	}
}

bool UAIMoveToInteractNode::HandleInteraction()
{
	bool bDidInteract = false;

	auto InteractComp = this->GetInteractionComponent();
	auto Interactable = this->GetInteractable();

	if (InteractComp->HasObject(Interactable))
	{
		if (this->CurrentData->Interaction.IsNone())
		{
			Interactable->InteractDefault(this->GetActorOwner(), this->CurrentData->Data);
		}
		else
		{
			Interactable->Interact(
				this->CurrentData->Interaction,
				this->GetActorOwner(),
				this->CurrentData->Data);
		}

		bDidInteract = true;
	}

	return bDidInteract;
}

bool UAIMoveToInteractNode::HasInteractable() const
{
	if (auto Component = this->GetInteractionComponent())
	{
		return Component->HasObject(this->GetInteractable());
	}
	else
	{
		return false;
	}
	
}

void UAIMoveToInteractNode::ComputeTarget()
{
	if (IsValid(this->CurrentData))
	{	
		if (auto Interactable = this->GetInteractable())
		{
			TArray<FVector> Locations;
			Interactable->GetInteractionPoints(Locations);

			if (Locations.Num() > 0 && !Interactable->bTravelToActor)
			{
				auto Dest = UPathFindingUtilsLibrary::ChooseNearLocation(this->GetAIController(), Locations);
				this->SetOverrideLocation(Dest);
			}
		}
	}
}

UInteractionSystem* UAIMoveToInteractNode::GetInteractionComponent() const
{
	return this->GetActorOwner()->FindComponentByClass<UInteractionSystem>();
}

void UAIMoveToInteractNode::BindEvents()
{
	if (auto Comp = this->GetInteractionComponent())
	{
		this->GetInteractionComponent()->OnInteractableAddedEvent.AddDynamic(
			this,
			&UAIMoveToInteractNode::OnInteractableAdded);
	}
	
}

void UAIMoveToInteractNode::OnInteractableAdded(UInteractableComponent* Interactable)
{
	if (Interactable == this->GetInteractable())
	{
		this->GetAIController()->StopMovement();
		this->EmitEvent(Events::AI::ARRIVE);
	}
}

UInteractableComponent* UAIMoveToInteractNode::GetInteractable() const
{
	if (this->CurrentData)
	{
		return this->CurrentData->Interactable->GetComponentByClass<UInteractableComponent>();
	}
	else
	{
		return nullptr;
	}
}

#if WITH_EDITOR

#endif