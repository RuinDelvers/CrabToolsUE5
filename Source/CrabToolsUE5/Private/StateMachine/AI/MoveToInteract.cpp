#include "StateMachine/AI/MoveToInteract.h"
#include "Navigation/PathFollowingComponent.h"
#include "StateMachine/Events.h"
#include "Components/Interaction/InteractionSystem.h"
#include "Components/Interaction/InteractableComponent.h"
#include "Utils/PathFindingUtils.h"

UAIInteractionData* UAIInteractionData::MakeInteractionData(
	FName InitInteraction,
	AActor* InitInteractable,
	UObject* InitData)
{
	auto Obj = NewObject<UAIInteractionData>(InitInteractable);

	Obj->Interaction = InitInteraction;
	Obj->Interactable = InitInteractable;
	Obj->Data = InitData;

	return Obj;
}

bool UAIInteractionData::IsValidData() const
{
	bool BaseValid = IsValid(this->Interactable);

	if (BaseValid)
	{
		return IsValid(this->Interactable->GetComponentByClass<UInteractableComponent>());
	}
	else
	{
		return false;
	}
}

UAIMoveToInteractNode::UAIMoveToInteractNode()
{
	this->AddEmittedEvent(Events::AI::CANNOT_INTERACT);
}

void UAIMoveToInteractNode::Initialize_Inner_Implementation()
{
	Super::Initialize_Inner_Implementation();

	check(IsValid(this->GetInteractionComponent()));
}

void UAIMoveToInteractNode::EnterWithData_Inner_Implementation(UObject* Data)
{
	if (auto Interaction = Cast<UAIInteractionData>(Data))
	{
		if (Interaction->IsValidData())
		{
			this->CurrentData = Interaction;
			this->BindEvents();
			Super::EnterWithData_Inner_Implementation(Interaction->Interactable);
		}
		else
		{
			Super::EnterWithData_Inner_Implementation(nullptr);
		}
	}
	else
	{
		Super::EnterWithData_Inner_Implementation(Data);
	}
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

	this->CurrentData = nullptr;

	this->GetInteractionComponent()->OnInteractableAddedEvent.RemoveAll(this);
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
	return this->GetInteractionComponent()->HasObject(this->GetInteractable());
}

void UAIMoveToInteractNode::ComputeTarget()
{
	if (IsValid(this->CurrentData))
	{		
		if (auto Interactable = this->GetInteractable())
		{
			TArray<FVector> Locations;
			Interactable->GetInteractionPoints(Locations);

			if (Locations.Num() > 0)
			{
				auto Dest = UPathFindingUtilsLibrary::ChooseNearLocation(this->GetAIController(), Locations);

				this->SetOverrideLocation(Dest);
			}
		}
		else
		{
			this->MoveData.DestinationActor = this->CurrentData->Interactable;
		}
	}
}

void UAIMoveToInteractNode::PostTransition_Inner_Implementation()
{
	if (!IsValid(this->MoveData.DestinationActor))
	{
		this->EmitEvent(Events::AI::CANNOT_INTERACT);
	}
	else if (!this->MoveData.DestinationActor->GetComponentByClass<UInteractableComponent>())
	{
		this->EmitEvent(Events::AI::CANNOT_INTERACT);
	} 
	else if (this->HasInteractable())
	{
		this->EmitEvent(Events::AI::ARRIVE);
	}
	else
	{
		this->ComputeTarget();
		Super::PostTransition_Inner_Implementation();
	}
}

UInteractionSystem* UAIMoveToInteractNode::GetInteractionComponent() const
{
	auto InteractComp = this->GetActorOwner()->FindComponentByClass<UInteractionSystem>();
	
	return InteractComp;
}

void UAIMoveToInteractNode::BindEvents()
{
	this->GetInteractionComponent()->OnInteractableAddedEvent.AddDynamic(this, &UAIMoveToInteractNode::OnInteractableAdded);
}

void UAIMoveToInteractNode::OnInteractableAdded(UInteractableComponent* Interactable)
{
	if (Interactable == this->GetInteractable())
	{
		this->EmitEvent(Events::AI::ARRIVE);
	}
}

UInteractableComponent* UAIMoveToInteractNode::GetInteractable() const
{
	return this->CurrentData->Interactable->GetComponentByClass<UInteractableComponent>();
}

#if WITH_EDITOR

#endif