#include "Components/Interaction/Interactable.h"

void UAbstractInteraction::PerformInteraction_Implementation()
{
	IInteractableInterface::Execute_InteractWithData(this->Interactable, this->Interactor, this);
}

void IInteractableInterface::GetLocations_Implementation(UPARAM(ref) TArray<FVector>& Locations)
{
	if (auto Actor = Cast<AActor>(this))
	{
		Locations.Add(Actor->GetActorLocation());
	}
}