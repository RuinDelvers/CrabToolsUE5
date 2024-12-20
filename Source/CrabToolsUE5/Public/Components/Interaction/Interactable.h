#pragma once

#include "CoreMinimal.h"
#include "Interactable.generated.h"

/**
 * Base interaction class; Contains the interactor only. Extend this to hold data for
 * specific interactions for interactables.
 */
UCLASS(Abstract, Blueprintable, Category = "Interaction")
class CRABTOOLSUE5_API UAbstractInteraction : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interaction", meta=(ExposeOnSpawn=true))
	TObjectPtr<AActor> Interactor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (ExposeOnSpawn = true))
	TObjectPtr<UObject> Interactable;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interaction")
	void PerformInteraction();
	virtual void PerformInteraction_Implementation();
};

USTRUCT(Blueprintable)
struct CRABTOOLSUE5_API FInteractionData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interaction")
	TSubclassOf<UAbstractInteraction> InteractionType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	FString ActionText;
};

UINTERFACE(MinimalAPI, Blueprintable)
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

class IInteractableInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interaction")
	void Interact(AActor* User);
	virtual void Interact_Implementation(AActor* User) {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	void InteractWithData(AActor* User, UObject* Data);
	virtual void InteractWithData_Implementation(AActor* User, UObject* Data) {  }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	TArray<FInteractionData> GetAvailableInteractions(AActor* User);
	virtual TArray<FInteractionData> GetAvailableInteractions_Implementation(AActor* User) { return {}; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	void EndInteract(AActor* User);
	virtual void EndInteract_Implementation(AActor* User) {}

	/* Returns the points where this interactable can be interacted with. Useful for AI pathing. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction",
		meta=(ForceAsFunction))
	void GetLocations(UPARAM(ref) TArray<FVector>& Locations);
	virtual void GetLocations_Implementation(UPARAM(ref) TArray<FVector>& Locations);
};