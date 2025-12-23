#pragma once

#include <limits>
#include "Utils/Enums.h"
#include "GameplayTagContainer.h"
#include "RPGComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStatusEvent, UStatus*, Status);

class URPGProperty;

/* Base class for all Status objects for the RPG System*/
UCLASS(Blueprintable, DefaultToInstanced, CollapseCategories, EditInlineNew, Within="RPGComponent")
class CRABTOOLSUE5_API UStatus: public UObject
{
	GENERATED_BODY()

private:

	UPROPERTY()
	int Stacks = 0;

	UPROPERTY()
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

	UFUNCTION(BlueprintCallable, Category = "Status")
	void PauseTimer();

	UFUNCTION(BlueprintCallable, Category = "Status")
	void UnpauseTimer();

	virtual UWorld* GetWorld() const override;	

protected:

	void Apply();
	void Remove();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "RPG|Status",
		meta=(DisplayName="Apply"))
	void Apply_Inner();
	virtual void Apply_Inner_Implementation() {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "RPG|Status",
		meta = (DisplayName = "Remove"))
	void Remove_Inner();
	virtual void Remove_Inner_Implementation() {}

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

	UPROPERTY(VisibleAnywhere, Category="RPG|Status")
	TMap<FGameplayTag, FStatusData> Statuses;

private:

	TArray<TObjectPtr<UStatus>> TickedStatuses;

public:

	UPROPERTY(BlueprintAssignable, Category = "Status")
	FStatusEvent OnStatusExpired;

	UPROPERTY(BlueprintAssignable, Category = "Status")
	FStatusEvent OnStatusRemoved;

	UPROPERTY(BlueprintAssignable, Category = "Status")
	FStatusEvent OnStatusAdded;

public:

	URPGComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "RPG")
	void TurnStart();

	UFUNCTION(BlueprintCallable, Category = "RPG")
	void TurnEnd();

	UFUNCTION(BlueprintCallable, Category = "RPG|Status", meta=(DeterminesOutputType="Status"))
	UStatus* MakeStatus(TSubclassOf<UStatus> Status);

	template <class T>
	T* MakeStatus()
	{
		auto Status = NewObject<T>(this, T::StaticClass());
		return Status;
	}

	template <class T>
	T* MakeStatus(UClass* StatusClass)
	{
		auto Status = NewObject<T>(this, StatusClass);
		return Status;
	}

	UFUNCTION(BlueprintCallable, Category = "RPG|Status")
	void GetStatus(FGameplayTag StatusID, UPARAM(Ref) TArray<UStatus*>& Found);

	UFUNCTION(BlueprintCallable, Category = "RPG|Status")
	URPGProperty* FindRPGPropertyByName(FName Ref, bool bRecurse=true) const;

	UFUNCTION(BlueprintCallable, Category = "RPG|Status")
	void PauseStatus();

	UFUNCTION(BlueprintCallable, Category = "RPG|Status")
	void UnpauseStatus();

	const FStatusData* GetStatusGroupData(FGameplayTag Group) const;

	UStatus* GetStatusInstanceFromGroup(FGameplayTag Group) const;

	TArray<FString> GetRPGPropertyNames(TSubclassOf<URPGProperty> Props, bool bRecurse=true) const;

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

	UFUNCTION(BlueprintCallable, Category = "RPG|Status")
	void ApplyStatus(UStatus* Status);

	UFUNCTION(BlueprintCallable, Category = "RPG|Status")
	void RemoveStatus(FGameplayTag StatusID, bool bAllInstances = true);

	UFUNCTION(BlueprintCallable, Category = "RPG|Status")
	void RemoveStatusInstance(UStatus* Status);

	virtual void BeginPlay() override;
	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

private:

	UFUNCTION()
	void OnStatusExpiredCallback(UStatus* Status);

	friend class UStatus;
};