#pragma once

#include "GameFramework/Actor.h"
#include "Ability/Ability.h"
#include "Ability/TargetingController.h"
#include "InlineTargeting.generated.h"

/* A simple targeting data type. This can be used for instantly specifying targets and using them*/
UCLASS(Blueprintable)
class CRABTOOLSUE5_API UInlineTargeting : public UObject, public ITargetingControllerInterface, public IHasAbilityInterface
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, Transient, meta = (AllowPrivateAccess, ExposeOnSpawn = true))
	TObjectPtr<AActor> UsingActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ability",
		meta = (ExposeOnSpawn))
	TObjectPtr<UAbility> AbilityOwner;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ability",
		meta = (ExposeOnSpawn))
	TArray<FTargetingData> Data;

public:

	UInlineTargeting(const FObjectInitializer& Init);

	virtual AActor* GetUsingActor_Implementation() const override { return this->UsingActor; }
	//virtual void Initialize_Implementation() override;
	//virtual void Confirm_Implementation() override;
	//virtual void AddListener_Implementation(const FConfirmTargetsSingle& Callback) override;
	//virtual void AddValidationListener_Implementation(const FValidateTargetingSingle& Callback) override;
	//virtual void AddDestroyedListener_Implementation(const FTargetingUpdated& Callback) override;
	//virtual void AddDisabledListener_Implementation(const FTargetingUpdated& Callback) override;
	virtual void GetTargetData_Implementation(TArray<FTargetingData>& OutData) const override;
	//virtual void PopTarget_Implementation() override;
	virtual void AddTarget_Implementation(const FTargetingData& TargetData) override;
	virtual int GetTargetCount_Implementation() const override { return this->Data.Num(); }

	//virtual void SetEnabled_Implementation(bool bNewEnabled) override;
	virtual bool GetEnabled_Implementation() const override { return false; }

	virtual void SetAbility_Implementation(UAbility* IAbiOwner) override { this->AbilityOwner = IAbiOwner; }
	virtual UAbility* GetAbility_Implementation() override { return this->AbilityOwner; }
};