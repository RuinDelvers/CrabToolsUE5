#pragma once

#include "Components/ActorComponent.h"
#include "CollisionQueryParams.h"
#include "MouseOverComponent.generated.h"

class FViewport;
class UDragDropOperation;

UCLASS(Blueprintable, ClassGroup = (General),
	meta = (BlueprintSpawnableComponent))
class CRABTOOLSUE5_API UMouseOverComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace", meta = (AllowPrivateAccess))
	TEnumAsByte<ECollisionChannel> Channel = ECC_Visibility;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace", meta = (AllowPrivateAccess))
	float MaxTraceDistance = 5000;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trace", meta = (AllowPrivateAccess))
	FHitResult Result;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trace", meta = (AllowPrivateAccess))
	TObjectPtr<AActor> MouseOverActor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace", meta = (AllowPrivateAccess))
	bool bDoMultiTrace = false;

	UPROPERTY(VisibleAnywhere, Category="Actors")
	TArray<TObjectPtr<AActor>> MousePointActors;

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

	UPROPERTY()
	TArray<FHitResult> MultiResults;

	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;

	FGeometry ViewportGeometry;
	FVector2D ViewportSize;

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
