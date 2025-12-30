#pragma once

#include "Logging/LogMacros.h"
#include "Utils/WorldAwareObject.h"
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

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Ability")
	void SetAbility(UAbility* Abi);
	virtual void SetAbility_Implementation(UAbility* Abi) { }
};

/* Base class for ability data. This is used as a simple data retrieval handle. */
UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced, Within="Ability")
class CRABTOOLSUE5_API UAbilityData : public UObject, public IHasAbilityInterface
{
	GENERATED_BODY()
	
public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDataChanged, UAbilityData*, Data);

	UPROPERTY(BlueprintAssignable, Category="Data")
	FDataChanged OnDataChanged;

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Data")
	void Initialize();
	virtual void Initialize_Implementation() {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Data")
	bool IsDynamic() const;
	virtual bool IsDynamic_Implementation() const { return false; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Targeting")
	float Range() const;
	virtual float Range_Implementation() const { return std::numeric_limits<float>::infinity(); }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Targeting")
	int TargetCount() const;
	virtual int TargetCount_Implementation() const { return 0; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Targeting")
	FName TargetAttachPoint() const;
	virtual FName TargetAttachPoint_Implementation() const { return NAME_None; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Targeting")
	USceneComponent* TargetAttachComponent() const;
	virtual USceneComponent* TargetAttachComponent_Implementation() const { return nullptr; }

	UFUNCTION(BlueprintCallable, Category = "Data")
	void Update();

	UFUNCTION(BlueprintCallable, Category = "Data")
	UAbility* GetAbilityOwner() const { return CastChecked<UAbility>(this->GetOuter()); }

	virtual UAbility* GetAbility_Implementation() override { return CastChecked<UAbility>(this->GetOuter()); }
};

UCLASS(Blueprintable)
class CRABTOOLSUE5_API UInlineAbilityData : public UAbilityData
{
	GENERATED_BODY()

private:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ability",
		meta = (AllowPrivateAccess))
	bool bIsDynamic = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Ability",
		meta = (AllowPrivateAccess))
	float AbilityRange;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ability",
		meta=(AllowPrivateAccess))
	int AbilityTargetCount = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ability",
		meta = (AllowPrivateAccess))
	FName AttachPoint = NAME_None;

	UPROPERTY(BlueprintReadWrite, Category = "Ability",
		meta = (AllowPrivateAccess))
	TObjectPtr<USceneComponent> AttachComponent;

public:

	UInlineAbilityData();
	
	virtual bool IsDynamic_Implementation() const override { return this->bIsDynamic; }
	virtual float Range_Implementation() const override { return this->AbilityRange; }
	virtual int TargetCount_Implementation() const override { return this->AbilityTargetCount; }
	virtual FName TargetAttachPoint_Implementation() const override { return this->AttachPoint; }

	void SetTargetAttachComponent(USceneComponent* NewComp) { this->AttachComponent = NewComp; }
};

/**
 * Class for player abilities. Abilities consist of a start, a continuous action,
 * a discrete action that happens on request, and an finish.
 */
UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class CRABTOOLSUE5_API UAbility : public UWorldAwareObject, public IHasAbilityInterface
{
	GENERATED_BODY()
	
	bool bActive = false;

	UPROPERTY(BlueprintReadOnly, Category="Ability", meta=(AllowPrivateAccess))
	bool bUseable = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "Ability", meta = (AllowPrivateAccess))
	TObjectPtr<UAbilityData> AbilityData;

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

	UAbility(const FObjectInitializer& ObjInit);

	UFUNCTION(BlueprintCallable, Category = "Ability")
	void Initialize(AActor* POwner);

	UFUNCTION(BlueprintCallable, Category = "Ability")
	void Detach();

	UFUNCTION(BlueprintCallable, Category="Ability")
	void Start();

	UFUNCTION(BlueprintCallable, Category="Ability")
	void Perform();

	UFUNCTION(BlueprintCallable, Category="Ability")
	void Tick(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Ability", meta=(DeterminesOutputType="DataClass"))
	UAbilityData* SetAbilityData(TSubclassOf<UAbilityData> DataClass);

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

	UFUNCTION(BlueprintCallable, Category = "Ability")
	UAbilityData* GetData() const { return this->AbilityData; }

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

	/* Returns the outer of this ability as an ability. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Ability")
	UAbility* GetParent() const;

	/* Returns the outer of this ability as an ability. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ability",
		meta=(DeterminesOutputType="ParentClass"))
	UAbility* GetParentAs(TSubclassOf<UAbility> ParentClass) const;

	virtual UAbility* GetAbility_Implementation() override { return this; }

protected:

	UFUNCTION(BlueprintNativeEvent, Category = "Ability",
		meta=(DisplayName="Detach"))
	void Detach_Inner();
	virtual void Detach_Inner_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, Category = "Ability",
		meta = (DisplayName = "Cancel"))
	void Cancel_Inner();
	virtual void Cancel_Inner_Implementation() {}

	/* Start the ability; Used for initializing data. */
	UFUNCTION(BlueprintNativeEvent, Category = "Ability",
		meta = (DisplayName = "Start"))
	void Start_Inner();
	virtual void Start_Inner_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, Category = "Ability",
		meta = (DisplayName = "Initialize"))
	void Initialize_Inner();
	virtual void Initialize_Inner_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, Category = "Ability",
		meta = (DisplayName = "Perform"))
	void Perform_Inner();
	virtual void Perform_Inner_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, Category = "Ability",
		meta = (DisplayName = "Tick"))
	void Tick_Inner(float DeltaTime);
	virtual void Tick_Inner_Implementation(float DeltaTime) {}

	UFUNCTION(BlueprintNativeEvent, Category = "Ability",
		meta = (DisplayName = "Finish"))
	void Finish_Inner();
	virtual void Finish_Inner_Implementation() {}

};
