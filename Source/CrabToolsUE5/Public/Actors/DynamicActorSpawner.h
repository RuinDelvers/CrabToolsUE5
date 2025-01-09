#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ChildActorComponent.h"
#include "DynamicActorSpawner.generated.h"

UCLASS(Abstract, Blueprintable)
class CRABTOOLSUE5_API ADynamicActorSpawner : public AActor
{
	GENERATED_BODY()

private:
	
	#if WITH_EDITORONLY_DATA
		UPROPERTY()
		TObjectPtr<AActor> PreviewActor;

		UPROPERTY()
		TObjectPtr<class UBillboardComponent> EditorSprite;
	#endif // WITH_EDITORONLY_DATA

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ActorSpawning", meta=(AllowPrivateAccess))
	TSoftClassPtr<AActor> ActorClass;	

public:	

	ADynamicActorSpawner();

	UFUNCTION(BlueprintCallable, Category="ActorSpawning")
	void SpawnActor();

	#if WITH_EDITOR
		void ToggleDisplay();
	#endif

protected:

	virtual void BeginPlay() override;

	#if WITH_EDITOR
		virtual void PostEditChangeProperty(struct FPropertyChangedEvent& Event) override;
		void ClearPreviews();
	#endif // WITH_EDITOR

private:

	FORCEINLINE AActor* GetActorObject() const { return Cast<AActor>(this->ActorClass->GetDefaultObject()); }
	
};
