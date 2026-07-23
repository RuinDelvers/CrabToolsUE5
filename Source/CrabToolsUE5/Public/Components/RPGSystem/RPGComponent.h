#pragma once

#include <limits>
#include "Utils/Enums.h"
#include "GameplayTagContainer.h"
#include "RPGComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStatusEvent, UStatus*, Status);

class URPGProperty;

UENUM(BlueprintType)
enum class EStatusTimingMethod: uint8
{
	/* No timing used at all. */
	NONE UMETA(DisplayName = "None"),
	/* Uses the game time, and timer implementation to handle duration of the timing. */
	GAME_TIME UMETA(DisplayName="Game Time"),
	/* Count the duration in turns, and only decrement when turn starts. */
	TURN_TIME_START UMETA(DisplayName = "Turn Time Start"),
	/* Count the duration in turns, and only decrement when turn ends. */
	TURN_TIME_END UMETA(DisplayName = "Turn Time End"),
};

USTRUCT(BlueprintType)
struct FStatusInitData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Init")
	TSubclassOf<UStatus> StatusClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Init")
	EStatusTimingMethod TimingMethod = EStatusTimingMethod::NONE;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Init")
	int MaxStacks = std::numeric_limits<int>::max();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Init")
	float MaxDuration = std::numeric_limits<float>::infinity();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Init")
	float InitDuration = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Init")
	int MaxTurns = std::numeric_limits<int>::max();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Init")
	int InitTurns = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Init")
	bool bInfiniteTurns = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Init")
	float TurnRealDuration = -1.0;
};


/* Base class for all Status objects for the RPG System*/
UCLASS(Blueprintable, DefaultToInstanced, CollapseCategories, EditInlineNew, Within="RPGComponent")
class CRABTOOLSUE5_API UStatus: public UObject
{
	GENERATED_BODY()

private:

	UPROPERTY(VisibleAnywhere, Category = "Status")
	FStatusInitData StatusData;

	UPROPERTY(VisibleAnywhere, Category="Status")
	int Stacks = 0;

	UPROPERTY(VisibleAnywhere, Category = "Status")
	int RemainingTurns = 0;

	UPROPERTY(VisibleAnywhere, Category = "Status")
	bool bAttached = false;

	FTimerHandle Timer;

public:

	UPROPERTY(BlueprintAssignable, Category="Status")
	FStatusEvent OnExpired;

	UPROPERTY(BlueprintAssignable, Category = "Status")
	FStatusEvent OnRemoved;

	UPROPERTY(BlueprintAssignable, Category = "Status")
	FStatusEvent OnAdded;

public:

	void Initialize(const FStatusInitData& InitData);

	bool IsAttached() const;
	void Refresh();
	void RefreshSum();

	UFUNCTION(BlueprintCallable, Category="Status")
	void Attach();

	UFUNCTION(BlueprintCallable, Category="Status")
	URPGComponent* GetOwner() const;

	UFUNCTION(BlueprintCallable, Category = "Status")
	float GetRemainingTime() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="RPG|Status")
	int GetStacks() const { return this->Stacks; }

	/* Returns a constant duration that is used when setting timers & stacking times. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "RPG|Status")
	float GetDuration() const;
	virtual float GetDuration_Implementation() const { return 0.0f; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "RPG|Status")
	FGameplayTag GetStatusGroup() const;
	virtual FGameplayTag GetStatusGroup_Implementation() const
		PURE_VIRTUAL(UStatus::GetStatusGroup_Implementation, return FGameplayTag(); );

	/* Used for real time handling.*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="RPG|Status")
	void Tick(float DeltaTime);
	virtual void Tick_Implementation(float DeltaTime) {}

	/* Called when real time turn ticking is active. This will be called during real time play when turns are decremented.*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "RPG|Status")
	void TickTurnReal();
	virtual void TickTurnReal_Implementation() {}

	/* Used for turn based calls. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "RPG|Status")
	void TurnStart();
	virtual void TurnStart_Implementation() {}

	/* Used for turn based calls. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "RPG|Status")
	void TurnEnd();
	virtual void TurnEnd_Implementation() {}

	UFUNCTION(BlueprintCallable, Category = "RPG|Status")
	void Stack(int Quantity = 1);

	/* Call this to remove the status from the RPG component. */
	UFUNCTION(BlueprintCallable, Category = "RPG|Status")
	void Detach();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "RPG|Status")
	bool RequiresTick() const;
	virtual bool RequiresTick_Implementation() const { return false; }

	/* Resets the timer to the duration specified by this status;*/
	UFUNCTION(BlueprintCallable, Category = "RPG|Status")
	void SetTimer();

	UFUNCTION(BlueprintCallable, Category = "RPG|Status")
	void AddTimer(float Amount);

	UFUNCTION(BlueprintCallable, Category = "RPG|Status")
	void AddTurns(int Amount);

	UFUNCTION(BlueprintCallable, Category = "Status")
	void PauseTimer();

	UFUNCTION(BlueprintCallable, Category = "Status")
	void UnpauseTimer();

	virtual UWorld* GetWorld() const override;	

protected:

	void Apply();

	/* This is called by the status when */
	void Removed();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "RPG|Status",
		meta=(DisplayName="Apply"))
	void Apply_Inner();
	virtual void Apply_Inner_Implementation() {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "RPG|Status",
		meta = (DisplayName = "Remove"))
	void Removed_Inner();
	virtual void Removed_Inner_Implementation() {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "RPG|Status",
		meta = (DisplayName = "Stack"))
	void Stack_Inner(int Quantity);
	virtual void Stack_Inner_Implementation(int Quantity) {}

private:

	UFUNCTION()
	void OnDurationExpired();

	friend class URPGComponent;
};

USTRUCT(BlueprintType)
struct FStatusData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Status",
		meta=(ShowInnerProperties))
	TArray<TObjectPtr<UStatus>> Instances;
};

/* Component that handles control, access, and manipualtion of Resources and Attributes in an RPG Setting.*/
UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced, CollapseCategories,
	ClassGroup=("GameSystems"), meta = (BlueprintSpawnableComponent))
class CRABTOOLSUE5_API URPGComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category="Status")
	TMap<FGameplayTag, FStatusData> Statuses;

	UPROPERTY(EditAnywhere, Category="Status")
	bool bTickStatus = true;

private:

	TArray<TObjectPtr<UStatus>> TickedStatuses;

public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTurnChangedEvent, URPGComponent*, Comp);

	UPROPERTY(BlueprintAssignable, Category = "Turns")
	FTurnChangedEvent OnTurnStart;

	UPROPERTY(BlueprintAssignable, Category = "Turns")
	FTurnChangedEvent OnTurnEnd;

	UPROPERTY(BlueprintAssignable, Category = "Status")
	FStatusEvent OnStatusExpired;

	UPROPERTY(BlueprintAssignable, Category = "Status")
	FStatusEvent OnStatusRemoved;

	UPROPERTY(BlueprintAssignable, Category = "Status")
	FStatusEvent OnStatusAdded;

public:

	URPGComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Tick")
	void TurnStart();

	UFUNCTION(BlueprintCallable, Category = "Tick")
	void TurnEnd();

	template <class T>
	T* MakeStatus(const FStatusInitData& InitData)
	{
		T* Status = NewObject<T>(this, InitData.StatusClass);
		Status->Initialize(InitData);
		return CastChecked<T>(Status);
	}

	UFUNCTION(BlueprintCallable, Category = "Status")
	void GetStatus(FGameplayTag StatusID, TArray<UStatus*>& Found);

	UFUNCTION(BlueprintCallable, Category = "Status")
	bool HasStatus(FGameplayTag StatusID) const;

	UFUNCTION(BlueprintCallable, Category = "Status")
	URPGProperty* FindRPGPropertyByName(FName Ref, bool bRecurse=true) const;
	URPGProperty* FindDefaultRPGPropertyByName(FName Ref) const;

	UFUNCTION(BlueprintCallable, Category = "Status")
	void PauseStatus();

	UFUNCTION(BlueprintCallable, Category = "Status")
	void UnpauseStatus();

	const FStatusData* GetStatusGroupData(FGameplayTag Group) const;

	UStatus* GetStatusInstanceFromGroup(FGameplayTag Group) const;

	TArray<FString> GetRPGPropertyNames(TSubclassOf<URPGProperty> Props, bool bRecurse = true) const;
	TArray<FString> GetRPGPropertyNamesFromInterface(TSubclassOf<UInterface> Props, bool bRecurse = true) const;

protected:

	template <class T>
	T* CreateDefaultResource(FName PropName, FName Min = NAME_None, FName Max = NAME_None)
	{
		T* Prop = this->CreateDefaultSubobject<T>(PropName);

		Prop->SetMinRef(Min);
		Prop->SetMaxRef(Max);

		return Prop;
	}

	/* Leaves minimum to be zero. */
	template <class T>
	T* CreateDefaultResource(FName PropName, FName Max) 
	{
		return this->CreateDefaultResource(PropName, NAME_None, Max);
	}

	template <class T>
	T* CreateDefaultResource(FName PropName, URPGProperty* MinProp, URPGProperty* MaxProp)
	{
		T* Prop = this->CreateDefaultSubobject<T>(PropName);

		Prop->SetMinProp(MinProp);
		Prop->SetMaxProp(MaxProp);

		return Prop;
	}

	UFUNCTION(BlueprintNativeEvent, Category = "Turns", meta = (DisplayName = "TurnEnd"))
	void TurnEnd_Inner();
	virtual void TurnEnd_Inner_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, Category = "Turns", meta = (DisplayName = "TurnStart"))
	void TurnStart_Inner();
	virtual void TurnStart_Inner_Implementation() {}

	UFUNCTION(BlueprintCallable, Category = "Status")
	void ApplyStatus(UStatus* Status);

	UFUNCTION(BlueprintCallable, Category = "Status")
	void RemoveStatus(FGameplayTag StatusID, bool bAllInstances = true);

	UFUNCTION(BlueprintCallable, Category = "Status")
	void RemoveStatusInstance(UStatus* Status);

	virtual void BeginPlay() override;
	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

private:

	TArray<FString> GetRPGPropertyNames(UClass* Props, bool bRecurse, bool bIsInterface) const;

	UFUNCTION()
	void OnStatusExpiredCallback(UStatus* Status);

	friend class UStatus;
};