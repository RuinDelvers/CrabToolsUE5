#pragma once

#include "Components/ActorComponent.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "Components/Interaction/Interactable.h"
#include "InteractionSystem.generated.h"

class UInteractableComponent;

/* 
 * Interaction System used to control which nearby objects one can interact with. This
 * is primarily useful for games which has a button to interact with a nearby object, e.g.
 * Ammo, Levers, or otherwise, and multiple of them can be available at a time.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CRABTOOLSUE5_API UInteractionSystem : public UActorComponent
{
	GENERATED_BODY()
	
	TArray<TWeakObjectPtr<UInteractableComponent>> InteractableObjects;
	int SelectedIndex = 0;

public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractionSelected, UInteractableComponent*, Selected);

	/* Called when a new interactable is made available. */
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FInteractionSelected OnInteractableAddedEvent;

	/* Called when an available interactable has been removed. */
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FInteractionSelected OnInteractableRemovedEvent;

	/* Called when the currently selected interactable has changed. */
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FInteractionSelected OnInteractableSelectedEvent;

	/* Called prior to the current interactable is interacted with. */
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FInteractionSelected OnInteractableActivatedEvent;

public:	

	UInteractionSystem();
	
	UFUNCTION(BlueprintCallable, Category="Interaction")
	void AddInteractable(UInteractableComponent* Obj);

	UFUNCTION(BlueprintCallable, Category="Interaction")
	void RemoveInteractable(UInteractableComponent* Obj);

	UFUNCTION(BlueprintCallable, Category="Interaction")
	void Interact(UObject* Data);

	UFUNCTION(BlueprintCallable, Category="Interaction")
	void InteractWith(AActor* Redirect, UObject* Data);
	
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void Cycle();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Interaction")
	int Num() { return this->InteractableObjects.Num(); }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Interaction")
	UInteractableComponent* GetObj(int index) { return index >= 0 && index < this->InteractableObjects.Num() ? this->InteractableObjects[index].Get() : nullptr; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Interaction")
	int IndexOf(UInteractableComponent* Obj);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void Select(UInteractableComponent* Obj);

	UFUNCTION(BlueprintCallable, Category = "Interaction", meta=(ExpandBoolAsExecs="bHasObject"))
	bool HasObject(UInteractableComponent* Obj, bool& bHasObject);
	bool HasObject(UInteractableComponent* Obj);

	UInteractableComponent* GetSelected();
};
