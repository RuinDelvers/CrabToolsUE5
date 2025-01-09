#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DynamicActorSpawner.generated.h"

UENUM(BlueprintType, Category = "Elements")
enum ESpawnerPreviewType : uint8
{
	STATIC_MESH         UMETA(DisplayName = "StaticMesh"),
	SKELETAL_MESH       UMETA(DisplayName = "SkeletalMesh"),
};

UCLASS(Abstract, Blueprintable)
class ADynamicActorSpawner : public AActor
{
	GENERATED_BODY()

private:
	
	#if WITH_EDITORONLY_DATA
		UPROPERTY()
		TObjectPtr<UStaticMeshComponent> StaticMeshPreview;

		UPROPERTY()
		TObjectPtr<USkeletalMeshComponent> SkeletalMeshPreview;
	#endif // WITH_EDITORONLY_DATA

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ActorSpawning", meta=(AllowPrivateAccess))
	TSoftClassPtr<AActor> ActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ActorSpawning", meta = (AllowPrivateAccess))
	TEnumAsByte<ESpawnerPreviewType> PreviewType;

public:	

	ADynamicActorSpawner();

	void SpawnActor() const;

protected:

	#if WITH_EDITOR
		virtual void PostEditChangeProperty(struct FPropertyChangedEvent& Event) override;
		void ClearPreviews();
	#endif // WITH_EDITOR

private:

	FORCEINLINE AActor* GetActorObject() const { return Cast<AActor>(this->ActorClass->GetDefaultObject()); }
	
};
