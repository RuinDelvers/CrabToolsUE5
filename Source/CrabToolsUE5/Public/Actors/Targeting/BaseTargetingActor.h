#pragma once

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

	UPROPERTY(BlueprintAssignable, Category="Targeting")
	FValidateTargeting OnValidateTargeting;

	UPROPERTY(BlueprintAssignable, Category = "Targeting")
	FTargetingUpdatedMulti OnEnabledUpdated;

	FConfirmTargetsMulti OnConfirmTargets;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Targeting", meta=(ExposeOnSpawn=true))
	int MaxTargetCount = 0;

	TArray<FTargetingData> Data;

	bool bEnabled = true;

public:

	ABaseTargetingActor();

	virtual void BeginPlay() override;

	/* ITargetControllerInterface functions */
	virtual AActor* GetUsingActor_Implementation() const override { return this->UsingActor; }
	virtual void Initialize_Implementation() override;
	virtual void Confirm_Implementation() override;
	virtual void AddListener_Implementation(const FConfirmTargetsSingle& Callback) override;
	virtual void AddValidationListener_Implementation(const FValidateTargetingSingle& Callback) override;
	virtual void AddDestroyedListener_Implementation(const FTargetingUpdated& Callback) override;
	virtual void AddDisabledListener_Implementation(const FTargetingUpdated& Callback) override;
	virtual void GetTargetData_Implementation(TArray<FTargetingData>& OutData) const override;
	virtual void PopTarget_Implementation() override;
	virtual void AddTarget_Implementation(const FTargetingData& TargetData) override;
	virtual int GetTargetCount_Implementation() const override { return this->Data.Num(); }

	virtual void SetEnabled_Implementation(bool bNewEnabled) override;
	virtual bool GetEnabled_Implementation() const override { return this->bEnabled; }
	/* END ITargetControllerInterface functions */

protected:

	/* Shortcut without needing to use the interface calls. */
	FORCEINLINE AActor* GetUsingActorNative() const { return this->UsingActor; }

	UFUNCTION(BlueprintCallable, Category="Targeting")
	void PerformTargetCountCheck();

};
