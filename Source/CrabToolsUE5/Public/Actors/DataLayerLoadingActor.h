#pragma once

#include "CoreMinimal.h"
#include "DataLayerLoadingActor.generated.h"

class UDataLayerAsset;
class UBoxComponent;

UCLASS(Blueprintable)
class ADataLayerLoadingActor : public AActor
{
	GENERATED_BODY()

	/* The layer to load when this actor is collided with initially.*/
	UPROPERTY(EditAnywhere, Category="WorldPartition")
	TObjectPtr<UDataLayerAsset> LoadedLayer;

	UPROPERTY(EditAnywhere, Category = "WorldPartition")
	TObjectPtr<UDataLayerInstance> LoadedInstance;

	UPROPERTY(EditAnywhere, Category = "WorldPartition")
	TObjectPtr<UBoxComponent> LoaderDetector;

	UPROPERTY(EditAnywhere, Category = "WorldPartition")
	TObjectPtr<UBoxComponent> PauseDetector;

public:	

	// Sets default values for this actor's properties
	ADataLayerLoadingActor();

public:

protected:

	UFUNCTION(BlueprintNativeEvent, Category = "Combat")
	void OnLoaderOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void OnLoaderOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintNativeEvent, Category = "Combat")
	void OnPauserOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void OnPauserOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
