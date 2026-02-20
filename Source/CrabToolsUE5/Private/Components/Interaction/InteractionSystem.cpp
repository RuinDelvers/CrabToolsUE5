#include "Components/Interaction/InteractionSystem.h"
#include "Components/Interaction/InteractableComponent.h"

UInteractionComponent::UInteractionComponent()
{
	this->PrimaryComponentTick.bCanEverTick = false;
}


void UInteractionComponent::AddInteractable(UInteractableComponent* Obj)
{
	if (IsValid(Obj))
	{
		this->InteractableObjects.AddUnique(Obj);
		this->OnInteractableAddedEvent.Broadcast(Obj);

		if (this->InteractableObjects.Num() == 1)
		{
			this->OnInteractableSelectedEvent.Broadcast(Obj);
		}
	}
}


void UInteractionComponent::RemoveInteractable(UInteractableComponent* Obj)
{

	if (IsValid(Obj))
	{
		bool bChangedSelection = false;

		if (this->InteractableObjects.Num() > 0)
		{
			if (this->InteractableObjects[this->SelectedIndex] == Obj)
			{
				bChangedSelection = true;
				this->SelectedIndex = 0;
			}
		}

		this->InteractableObjects.Remove(Obj);
		this->OnInteractableRemovedEvent.Broadcast(Obj);

		if (bChangedSelection && this->InteractableObjects.Num() > 0)
		{
			this->OnInteractableSelectedEvent.Broadcast(this->GetSelected());
		}
	}
}


void UInteractionComponent::Interact(UObject* Data) {
	this->InteractWith(this->GetOwner(), Data);
}


void UInteractionComponent::InteractWith(AActor* Redirect,UObject* Data)
{
	if (this->InteractableObjects.Num() > 0)
	{
		if (auto Interactable = this->GetSelected())
		{
			Interactable->InteractDefault(Redirect->GetComponentByClass<UInteractionComponent>(), Data);
		}
	}
}

void UInteractionComponent::Cycle()
{
	if (this->InteractableObjects.Num() == 0) { return; }

	this->SelectedIndex = (this->SelectedIndex + 1) % this->InteractableObjects.Num();

	this->OnInteractableSelectedEvent.Broadcast(this->GetSelected());
}

UInteractableComponent* UInteractionComponent::GetSelected()
{
	if (this->InteractableObjects.Num() > 0)
	{
		auto& WeakVar = this->InteractableObjects[this->SelectedIndex];

		if (WeakVar.IsValid())
		{
			return WeakVar.Get();
		}
		else
		{
			this->InteractableObjects.RemoveAt(this->SelectedIndex);
			this->SelectedIndex = 0;
			return this->GetSelected();
		}
	}

	return nullptr;
}

int UInteractionComponent::IndexOf(UInteractableComponent* Obj)
{
	return this->InteractableObjects.IndexOfByPredicate([&](const TWeakObjectPtr<UObject> TObj)
		{
			return 	TObj.IsValid() && TObj.Get() == Obj;
		});
}

void UInteractionComponent::Select(UInteractableComponent* Obj)
{
	this->SelectedIndex = this->InteractableObjects.IndexOfByPredicate([&](const TWeakObjectPtr<UObject> TObj)
		{
			return 	TObj.IsValid() && TObj.Get() == Obj;
		});
}

bool UInteractionComponent::HasObject(UInteractableComponent* Obj, bool& bHasObject)
{
	auto index = this->InteractableObjects.IndexOfByPredicate([&](const TWeakObjectPtr<UObject> TObj)
		{
			return 	TObj.IsValid() && TObj.Get() == Obj;
		});

	bool Check = index >= 0;
	bHasObject = Check;
	return Check;
}

bool UInteractionComponent::HasObject(UInteractableComponent* Obj)
{
	bool Check = false;

	return this->HasObject(Obj, Check);
}