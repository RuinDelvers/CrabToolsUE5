#pragma once

#include "Components/SceneComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Widgets/ContextMenuInterface.h"
#include "InteractableComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FActorInteraction, AActor*, Interactor, UObject*, Data);

USTRUCT(BlueprintType)
struct FInteractableStateData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Interaction")
	bool bIsEnabled = true;
};

USTRUCT()
struct FActorInteractionData
{
	GENERATED_BODY()

	FActorInteraction Interaction;

	UPROPERTY(EditAnywhere, Category="Interaction", meta=(ShowOnlyInnerProperties))
	FInteractableStateData State;

	UPROPERTY(EditAnywhere, Category = "Interaction")
	bool bUseMoveLogic = false;

	UPROPERTY(EditAnywhere, Category = "Interaction")
	FContextMenuDisplayData UIData;
};

/*
 * Component which tracks the interaction callbacks from an actor.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CRABTOOLSUE5_API UInteractableComponent : public USceneComponent, public IContextMenuInterface
{
	GENERATED_BODY()

private:	

	UPROPERTY(EditDefaultsOnly, Category="Interaction", meta=(GetOptions="GetDefaultOptions"))
	FName DefaultInteraction;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction", meta = (GetKeyOptions = "GetInteractionOptions"))
	TMap<FName, FActorInteractionData> Interactions;
	TArray<TObjectPtr<UPrimitiveComponent>> InteractionPoints;

	UPROPERTY()
	TSet<TObjectPtr<AActor>> ValidActors;
	
public:

	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FActorInteraction OnInteractionEnded;

public:

	static TArray<FString> GetActorInteractions(UClass* Class);

public:

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetDefaultInteraction(FName NewDefaultInteraction);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetInteractionState(FName Interaction, const FInteractableStateData& NewData);

	UFUNCTION(BlueprintCallable, Category="Interaction")
	void InteractDefault(AActor* Interactor, UObject* Data);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void Interact(FName Interaction, AActor* Interactor, UObject* Data);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void EndInteraction(AActor* Interactor, UObject* Data);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Interaction")
	void GetInteractionPoints(TArray<FVector>& Points) const;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetInteractionEnabled(FName Interaction, bool bNewEnabled);

	virtual TArray<UContextMenuEntry*> GatherEntries_Implementation(AActor* Requester = nullptr) override;

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBeginActorOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void OnEndActorOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void MoveToInteract(FName Interaction, AActor* Interactor, UObject* Data);

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

/**
 * Intermediate data for interactions.
 */
UCLASS(Blueprintable, Category = "Interaction")
class CRABTOOLSUE5_API UAIInteractionData : public UObject
{
	GENERATED_BODY()

public:

	/* Convenience function for quickly makine of these object in a pure manner. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Interaction")
	static UAIInteractionData* MakeInteractionData(
		FName InitInteraction,
		AActor* InitInteractable,
		UObject* InitData);

public:

	/* Name of the interaction. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (ExposeOnSpawn = true))
	FName Interaction;

	/* The actor to perform the interaction upon. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (ExposeOnSpawn = true))
	TObjectPtr<AActor> Interactable;

	/* The data payload. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (ExposeOnSpawn = true))
	TObjectPtr<UObject> Data;

public:

	bool IsValidData() const;
};