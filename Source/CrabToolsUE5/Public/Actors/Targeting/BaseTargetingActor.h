#pragma once

#include "GameFramework/Actor.h"
#include "Ability/Ability.h"
#include "Ability/TargetingController.h"
#include "BaseTargetingActor.generated.h"

UCLASS(Abstract, Blueprintable, CollapseCategories, ClassGroup = (Custom),
	Within="BaseTargetingActor",
	meta = (BlueprintSpawnableComponent))
class CRABTOOLSUE5_API UTargetFilterComponent : public UActorComponent
{
	GENERATED_BODY()

private:

	UPROPERTY(EditDefaultsOnly, Category="Targeting")
	int Priority = 0;

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category=Targeting)
	bool Filter(const FTargetingData& Data, FText& FailureReason);
	virtual bool Filter_Implementation(const FTargetingData& Data, FText& FailureReason) { return true; }

	FORCEINLINE int GetPriority() const { return this->Priority; }
};

UCLASS(Abstract, Blueprintable)
class ABaseTargetingActor : public AActor, public ITargetingControllerInterface, public IHasAbilityInterface
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Transient, meta=(AllowPrivateAccess, ExposeOnSpawn=true))
	TObjectPtr<AActor> UsingActor;

public:

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category="Targeting")
	FValidateTargeting OnValidateTargeting;

	UPROPERTY(BlueprintAssignable, Category = "Targeting")
	FTargetingUpdatedMulti OnEnabledUpdated;

	FConfirmTargetsMulti OnConfirmTargets;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Ability",
		meta=(ExposeOnSpawn))
	TObjectPtr<UAbility> AbilityOwner;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Targeting", meta=(ExposeOnSpawn=true))
	int MaxTargetCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting", meta = (ExposeOnSpawn = true))
	bool bInitUsingAbility = false;

	TArray<FTargetingData> Data;
	mutable TArray<TObjectPtr<UTargetFilterComponent>> Filters;

	mutable bool bFoundFilters = false;
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

	virtual void SetAbility_Implementation(UAbility* IAbiOwner) override { this->AbilityOwner = IAbiOwner; }
	virtual UAbility* GetAbility_Implementation() override { return this->AbilityOwner; }

	bool ApplyFilters(const FTargetingData& Data, FText& FailureReason) const;

	const TArray<UTargetFilterComponent*>& GetFilters() const;

protected:

	/* Shortcut without needing to use the interface calls. */
	FORCEINLINE AActor* GetUsingActorNative() const { return this->UsingActor; }

	UFUNCTION(BlueprintCallable, Category="Targeting")
	void PerformTargetCountCheck();

	UFUNCTION()
	void InitFromAbility(UAbilityData* AbiData);

};
