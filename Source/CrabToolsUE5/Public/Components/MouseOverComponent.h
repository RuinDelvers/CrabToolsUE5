#pragma once

#include "Components/ActorComponent.h"
#include "CollisionQueryParams.h"
#include "MouseOverComponent.generated.h"

class FViewport;
class UDragDropOperation;
class UMouseOverComponent;

UENUM(BlueprintType)
enum class EMousePointActorPlacementAction : uint8
{
	/* Hide the actor. */
	HIDDEN        UMETA(DisplayName="Hidden"),
	HIDDEN_ATTACH UMETA(DisplayName = "Hide and Attach"),
	PLACE         UMETA(DisplayName = "Place"),
	LEAVE         UMETA(DisplayName = "Leave"),
};

/*
 * This interface is used by the MouseOverComponent to handle mouse point actors. Mouse point 
 * actors can be used for a variety of applications, dragging objects for example. This interface
 * defines the interface and behaviour that the MouseOverComponent will do for each actor.
 */
UINTERFACE(MinimalAPI, Blueprintable, BlueprintType)
class UMousePointActorInterface : public UInterface
{
	GENERATED_BODY()
};

class IMousePointActorInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, Category="MouseOver")
	void SetMouseOver(UMouseOverComponent* Component);
	virtual void SetMouseOver_Implementation(UMouseOverComponent* Component) {}

	UFUNCTION(BlueprintNativeEvent, Category = "MouseOver")
	void BeginPlacement();
	virtual void BeginPlacement_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, Category = "MouseOver")
	void EndPlacement();
	virtual void EndPlacement_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, Category = "MouseOver")
	bool ReadjustActor() const;
	virtual bool ReadjustActor_Implementation() const { return false; }

	UFUNCTION(BlueprintNativeEvent, Category = "MouseOver")
	EMousePointActorPlacementAction OnInvalidLocation();
	virtual EMousePointActorPlacementAction OnInvalidLocation_Implementation() { return EMousePointActorPlacementAction::HIDDEN; }

	UFUNCTION(BlueprintNativeEvent, Category = "MouseOver")
	EMousePointActorPlacementAction OnReadjustFailure();
	virtual EMousePointActorPlacementAction OnReadjustFailure_Implementation() { return EMousePointActorPlacementAction::HIDDEN; }
};

UCLASS(Blueprintable, ClassGroup = (General),
	meta = (BlueprintSpawnableComponent))
class CRABTOOLSUE5_API UMouseOverComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace", meta = (AllowPrivateAccess))
	TEnumAsByte<ECollisionChannel> Channel = ECC_Visibility;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace", meta = (AllowPrivateAccess))
	float MaxTraceDistance = 5000;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trace", meta = (AllowPrivateAccess))
	FHitResult Result;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trace", meta = (AllowPrivateAccess))
	TObjectPtr<AActor> MouseOverActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace", meta = (AllowPrivateAccess))
	bool bDoMultiTrace = false;

	UPROPERTY(VisibleAnywhere, Category="Actors")
	TArray<TObjectPtr<AActor>> MousePointActors;

	/* This will adjust the mouse point actor to prevent overlaps.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actors",
		meta=(AllowPrivateAccess))
	bool bAdjustMousePointActors = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actors",
		meta = (AllowPrivateAccess))
	float AdjustBias = 0.f;

	UPROPERTY(VisibleAnywhere, Category = "Actors")
	bool bOverridePosition = false;

	UPROPERTY(VisibleAnywhere, Category = "Actors")
	FVector2D OverrideLocation;

	UPROPERTY(VisibleAnywhere, Category = "Tick")
	bool bShouldTick = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tick",
		meta=(AllowPrivateAccess))
	bool bIsMousingUI = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tick",
		meta=(AllowPrivateAccess))
	bool bTickWhenMousingUI = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tick",
		meta = (AllowPrivateAccess))
	bool bClearTargetOnTickOff = true;

	FCollisionQueryParams Params;

	#if WITH_EDITORONLY_DATA
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace", meta = (AllowPrivateAccess))
		bool bDrawDebugLine = false;
	#endif

	UPROPERTY()
	TObjectPtr<APawn> PawnOwner;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Trace",
		meta=(AllowPrivateAccess))
	TArray<FHitResult> MultiResults;

	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;

	FGeometry ViewportGeometry;
	FVector2D ViewportSize;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trace",
		meta = (AllowPrivateAccess))
	FVector EndPoint;

	/* Current hit result that is being processed */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Trace",
		meta=(AllowPrivateAccess))
	FHitResult CurrentCheckResult;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Trace",
		meta=(AllowPrivateAccess))
	int CurrentResultIndex = 0;

public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMouseOverActor, AActor*, Actor);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMouseOverTick, UMouseOverComponent*, Component);

	UPROPERTY(BlueprintAssignable, Category="MouseOver")
	FMouseOverActor OnMouseOverActor;

	UPROPERTY(BlueprintAssignable, Category = "MouseOver")
	FMouseOverTick OnMouseOverTick;


public:

	UMouseOverComponent();

	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category = "Trace",
		meta=(ExpandBoolAsExecs="ReturnValue"))
	bool HasValidLocation() const { return IsValid(this->MouseOverActor); }

	UFUNCTION(BlueprintCallable, Category="Trace")
	void DoTrace();

	UFUNCTION(BlueprintCallable, Category = "Trace")
	FTransform GetPlacementTransform() const;

	UFUNCTION(BlueprintCallable, Category = "Trace", meta=(DeterminesOutputType="ActorType", DisplayName="AddMousePointActorClass"))
	AActor* AddMousePointActor(TSubclassOf<AActor> ActorType);

	UFUNCTION(BlueprintCallable, Category = "Trace", meta = (DeterminesOutputType = "ActorType", DisplayName = "AddMousePointActor"))
	void AddMousePointActorInstance(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "Trace")
	void RemoveMousePointActor(AActor* ActorToRemove);

	UFUNCTION(BlueprintCallable, Category = "Trace")
	void ClearMousePointActors();

	UFUNCTION(BlueprintCallable, Category = "Trace")
	void SetMousePointActorsHidden(bool bHidden);

	#if WITH_EDITOR
		UFUNCTION(BlueprintCallable, Category="Debug")
		void DisplayDebug();
	#endif

	UFUNCTION(BlueprintCallable, Category = "Trace")
	void SetOverridePosition(const FPointerEvent& NewOverride, bool bUpdateTraces=false);

	UFUNCTION(BlueprintCallable, Category = "Trace")
	void ClearOverride();

	UFUNCTION(BlueprintCallable, Category = "Trace")
	void SetIsMousingUI(bool bNewMousing);

	UFUNCTION(BlueprintCallable, Category = "Trace")
	void ClearTarget() { this->ReplaceActors(nullptr); }

	UFUNCTION(BlueprintCallable, Category = "Trace")
	void PlaceActorAtLocation(AActor* PlacedActor, bool bUseNormal=true) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Trace")
	bool IsOnNavMesh() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Trace")
	FVector GetLocation() const { return this->Result.ImpactPoint; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Trace")
	FVector GetNormal() const { return this->Result.ImpactNormal; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Items")
	bool IsWithinDistance(float Distance) const;

	virtual void SetComponentTickEnabled(bool bNewEnabled) override;

	const TArray<FHitResult>& GetMultiResults() const { return this->MultiResults; }
	const FHitResult& GetCurrentCheckResult() const;

	UFUNCTION(BlueprintCallable, Category = "Trace")
	void SetTraceComplex(bool bTraceComplex);

protected:

	UFUNCTION(BlueprintNativeEvent, Category = "Trace")
	bool ValidateActor(AActor* CheckActor);
	virtual bool ValidateActor_Implementation(AActor* CheckActor) { return true; }

	UFUNCTION(BlueprintNativeEvent, Category="MouseOver")
	void OnMouseOff(AActor* OldActor);
	virtual void OnMouseOff_Implementation(AActor* OldActor) {}

	UFUNCTION(BlueprintNativeEvent, Category = "MouseOver")
	void OnMouseOn(AActor* OldActor);
	virtual void OnMouseOn_Implementation(AActor* OldActor) {}

	virtual void BeginPlay() override;
	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction)
		override;

private:

	void OnViewportResized(FViewport* Viewport, uint32 Index);

	void UpdateViewportData();
	bool ProcessResult(const FHitResult& CheckResult, bool bIsMulti=false);
	void ReplaceActors(AActor* NewActor);
	void UpdateResult(const FHitResult& CheckResult);

	void UpdateMousePointActors();
	void UpdateTickingState();

	UFUNCTION()
	void OnControllerChanged(APawn* Pawn, AController* OldController, AController* NewController);
};
