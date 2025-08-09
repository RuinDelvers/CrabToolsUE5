#pragma once

#include "Components/SceneComponent.h"
#include "Components/PrimitiveComponent.h"
#include "InteractableComponent.generated.h"


/*
 * Component which tracks the interaction callbacks from an actor.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CRABTOOLSUE5_API UInteractableComponent : public USceneComponent
{
	GENERATED_BODY()

private:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInteraction, AActor*, Interactor, UObject*, Data);

	UPROPERTY(EditDefaultsOnly, Category="Interaction", meta=(GetOptions="GetDefaultOptions"))
	FName DefaultInteraction;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction", meta = (GetOptions = "GetInteractionOptions"))
	TSet<FName> Interactions;

	TMap<FName, FInteraction> InteractionCallbacks;
	TArray<TObjectPtr<UPrimitiveComponent>> InteractionPoints;

public:

	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FInteraction OnInteractionEnded;

public:

	UFUNCTION(BlueprintCallable, Category="Interaction")
	void InteractDefault(AActor* Interactor, UObject* Data);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void Interact(FName Interaction, AActor* Interactor, UObject* Data);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void EndInteraction(AActor* Interactor, UObject* Data);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Interaction")
	void GetInteractionPoints(TArray<FVector>& Points) const;

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBeginActorOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void OnEndActorOverlap(AActor* OverlappedActor, AActor* OtherActor);

private:

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	UFUNCTION()
	TArray<FString> GetDefaultOptions() const;

	UFUNCTION()
	TArray<FString> GetInteractionOptions() const;

	UFUNCTION()
	void ExampleInteraction(AActor* Interactor, UObject* Data) {}

#endif
};