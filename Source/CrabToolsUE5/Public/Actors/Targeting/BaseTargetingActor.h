#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Ability/TargetingController.h"
#include "BaseTargetingActor.generated.h"

UCLASS(Abstract, Blueprintable)
class ABaseTargetingActor : public AActor, public ITargetingControllerInterface
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Transient, meta=(AllowPrivateAccess, ExposeOnSpawn=true))
	TObjectPtr<AActor> UsingActor;


public:

	FConfirmTargetsMulti OnConfirmTargets;

public:

	ABaseTargetingActor();

	virtual void BeginPlay() override;

	/* ITargetControllerInterface functions */
	virtual AActor* GetUsingActor_Implementation() const override { return this->UsingActor; }
	virtual void Initialize_Implementation() override;
	virtual void Confirm_Implementation() override;
	virtual void AddListener_Implementation(const FConfirmTargetsSingle& Callback) override;
	/* END ITargetControllerInterface functions */

protected:

	/* Shortcut without needing to use the interface calls. */
	FORCEINLINE AActor* GetUsingActorNative() const { return this->UsingActor; }
};
