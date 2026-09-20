#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "InstancedStaticMeshActor.generated.h"

UCLASS(Blueprintable, meta=(PrioritizeCategories="DoorActor"))
class AInstancedStaticMeshActor : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InstancedStaticMeshActor",
		meta=(AllowPrivateAccess=true, ShowInnerProperties, ShowOnlyInnerProperties))
	TObjectPtr<UInstancedStaticMeshComponent> InstanceManager;

public:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category="InstancedStaticMeshActor",
		meta=(MakeEditWidget))
	TArray<FTransform> InstanceTransforms;


	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "InstancedStaticMeshActor")
	bool bUseWorldTransforms = false;
public:

	AInstancedStaticMeshActor();
	virtual void PostActorCreated() override;

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "InstancedStaticMeshActor")
	void RespawnInstances();

	#if WITH_EDITOR
		void PostEditChangeProperty(FPropertyChangedEvent& Event) override;

		UFUNCTION(BlueprintCallable, Category = "InstancedStaticMeshActor")
		void SetInstances(TArray<AStaticMeshActor*> MeshActors);

		UFUNCTION(BlueprintCallable, CallInEditor, Category = "InstancedStaticMeshActor")
		void BreakOutInstances();
	#endif
};
