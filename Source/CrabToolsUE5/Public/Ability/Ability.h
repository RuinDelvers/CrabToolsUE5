#pragma once

#include "CoreMinimal.h"
#include "Delegates/DelegateSignatureImpl.inl"
#include "Actors/Targeting/BaseTargetingActor.h"
#include "Logging/LogMacros.h"
#include "Ability.generated.h"

CRABTOOLSUE5_API DECLARE_LOG_CATEGORY_EXTERN(LogAbility, Log, All);

/* This interface is used for containers of abilitys, e.g. Targeting Controllers. */
UINTERFACE(MinimalAPI, Blueprintable, BlueprintType)
class UHasAbilityInterface : public UInterface
{
	GENERATED_BODY()
};

class IHasAbilityInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Ability")
	UAbility* GetAbility();
	virtual UAbility* GetAbility_Implementation() { return nullptr; }
};

/**
 * Class for player abilities. Abilities consist of a start, a continuous action,
 * a discrete action that happens on request, and an finish.
 */
UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class CRABTOOLSUE5_API UAbility : public UObject, public IHasAbilityInterface
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, Transient, DuplicateTransient,
		meta=(ExposeOnSpawn=true, AllowPrivateAccess))
	TObjectPtr<AActor> Owner;

	bool bActive = false;
	UPROPERTY(BlueprintReadOnly, Category="Ability", meta=(AllowPrivateAccess))
	bool bUseable = true;

	UPROPERTY(Transient)
	TObjectPtr<ABaseTargetingActor> Targeting;

public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityStarted, UAbility*, Ability);
	UPROPERTY(BlueprintAssignable, Category = "Ability")
	FAbilityStarted OnAbilityStarted;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityPerformed, UAbility*, Ability);

	/* Called when the ability performs its body. */
	UPROPERTY(BlueprintAssignable, Category="Ability")
	FAbilityStarted OnAbilityPerformed;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityFinished, UAbility*, Ability);
	UPROPERTY(BlueprintAssignable, Category = "Ability")
	FAbilityStarted OnAbilityFinished;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityCanceled, UAbility*, Ability);
	UPROPERTY(BlueprintAssignable, Category = "Ability")
	FAbilityCanceled OnAbilityCanceled;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAbilityUseabilityChanged, UAbility*, Ability, bool, bNewUseability);
	UPROPERTY(BlueprintAssignable, Category = "Ability")
	FAbilityUseabilityChanged OnAbilityUseabilityChanged;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityDeleted, UAbility*, Ability);
	UPROPERTY(BlueprintAssignable, Category = "Ability")
	FAbilityDeleted OnAbilityDeleted;

public:

	UFUNCTION(BlueprintCallable, Category = "Ability")
	void Initialize(AActor* POwner);

	UFUNCTION(BlueprintCallable, Category="Ability")
	void Start();

	UFUNCTION(BlueprintCallable, Category="Ability")
	void Perform();

	UFUNCTION(BlueprintCallable, Category="Ability")
	void Tick(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Ability")
	void SetUseable(bool bNewUseable);
	bool GetUseable() const { return this->bUseable; }

	/* Call this to finish the ability if you are the controller. */
	UFUNCTION(BlueprintCallable, Category="Ability")
	void Finish();

	/* Called when the ability was canceled. Can be used even when not active. */
	UFUNCTION(BlueprintCallable, Category = "Ability")
	void Cancel();

	UFUNCTION(BlueprintCallable, Category = "Ability")
	void Delete();

	UFUNCTION(BlueprintNativeEvent, Category = "Ability")
	void Delete_Inner();
	virtual void Delete_Inner_Implementation() {}

	UFUNCTION(BlueprintCallable, Category = "Ability")
	bool IsActive() const { return this->bActive; }

	/* Start the ability; Used for initializing data. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ability")
	bool RequiresTick() const;
	virtual bool RequiresTick_Implementation() const { return false; }

	UFUNCTION(BlueprintCallable, Category = "Ability")
	AActor* GetOwner() const;

	virtual UWorld* GetWorld() const override;

	/* Returns the outer of this ability as an ability. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Ability")
	UAbility* GetParent() const;

	/* Returns the outer of this ability as an ability. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ability",
		meta=(DeterminesOutputType="ParentClass"))
	UAbility* GetParentAs(TSubclassOf<UAbility> ParentClass) const;

	virtual UAbility* GetAbility_Implementation() override { return this; }

protected:

	UFUNCTION(BlueprintNativeEvent, Category = "Ability")
	void Cancel_Inner();
	void Cancel_Inner_Implementation() {}

	/* Start the ability; Used for initializing data. */
	UFUNCTION(BlueprintNativeEvent, Category = "Ability")
	void Start_Inner();
	virtual void Start_Inner_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, Category = "Ability")
	void Initialize_Inner();
	virtual void Initialize_Inner_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, Category = "Ability")
	void Perform_Inner();
	virtual void Perform_Inner_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, Category = "Ability")
	void Tick_Inner(float DeltaTime);
	virtual void Tick_Inner_Implementation(float DeltaTime) {}

	UFUNCTION(BlueprintNativeEvent, Category = "Ability")
	void Finish_Inner();
	virtual void Finish_Inner_Implementation() {}

};
