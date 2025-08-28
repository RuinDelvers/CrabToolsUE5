#pragma once

#include <limits>
#include "Utils/Enums.h"
#include "GameplayTagContainer.h"
#include "RPGComponent.generated.h"

class URPGProperty;

UENUM(BlueprintType, Category = "WoN|Interactions")
enum class EStatusStackType : uint8
{
	/* The status type stacks incrementally. */
	STACK UMETA(DisplayName = "Stacking"),

	/* The status is unique amongst its StackID. */
	UNIQUE UMETA(DisplayName = "Unique"),

	/* Add the status to the component, but don't stack, refresh, or ignore. */
	ADD UMETA(DisplayName = "Add"),

	/* Whether or not stacking the status will refresh its duration to the max of remaining and its duration rule. */
	REFRESH UMETA(DisplayName = "Refresh"),

	REFRESH_SUM UMETA(DisplayName = "Refresh Sum"),
};

/* Base class for all Status objects for the RPG System*/
UCLASS(Blueprintable, DefaultToInstanced, CollapseCategories, EditInlineNew, Within="RPGComponent")
class CRABTOOLSUE5_API UStatus: public UObject
{
	GENERATED_BODY()

private:

	UPROPERTY()
	FGameplayTag StatusID;

	UPROPERTY()
	int Stacks = 0;

	UPROPERTY()
	bool bAttached = false;

	FTimerHandle Timer;

public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStatusEvent, UStatus*, Status);
	UPROPERTY(BlueprintAssignable, Category="Status")
	FStatusEvent OnExpired;

	UPROPERTY(BlueprintAssignable, Category = "Status")
	FStatusEvent OnRemoved;

public:

	void Refresh();
	void RefreshSum();

	UFUNCTION(BlueprintCallable, Category="Status")
	void Attach(int InitStacks);

	UFUNCTION(BlueprintCallable, Category="Status")
	URPGComponent* GetOwner() const;

	UFUNCTION(BlueprintCallable, Category = "Status")
	float GetRemainingTime() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="RPG|Status")
	int GetStacks() const { return this->Stacks; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "RPG|Status")
	float GetDuration() const;
	virtual float GetDuration_Implementation() const { return 0.0f; }

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

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "RPG|Status")
	EStatusStackType GetStackType() const;
	virtual EStatusStackType GetStackType_Implementation() const { return EStatusStackType::ADD; }

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
	void Stack_Inner(int Quantity = 1);
	virtual void Stack_Inner_Implementation(int Quantity = 1) {}

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

/*
 * This class is used by RPG components to query data for statuses based off of their
 * StatusID GameplayTag. This is useful for many status types which are handled
 * based off of data.
 */
UCLASS(Abstract, Blueprintable)
class CRABTOOLSUE5_API UStatusDataRepo : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, Category = "Status")
	bool HasStatusData(FGameplayTag StatusID) const;
	virtual bool HasStatusData_Implementation(FGameplayTag StatusID) const { return false; }

	UFUNCTION(BlueprintNativeEvent, Category="Status")
	EStatusStackType StackType(FGameplayTag StatusID) const;
	virtual EStatusStackType StackType_Implementation(FGameplayTag StatusID) const { return EStatusStackType::ADD; }

	UFUNCTION(BlueprintNativeEvent, Category = "Status")
	UStatus* ConstructStatus(FGameplayTag StatusID, URPGComponent* Outer) const;
	virtual UStatus* ConstructStatus_Implementation(FGameplayTag StatusID, URPGComponent* Outer) const { return nullptr; }
};

/* Component that handles control, access, and manipualtion of Resources and Attributes in an RPG Setting.*/
UCLASS(Blueprintable, CollapseCategories, ClassGroup=(Custom),
	meta=(BlueprintSpawnableComponent))
class CRABTOOLSUE5_API URPGComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category="RPG|Status")
	TMap<FGameplayTag, FStatusData> Statuses;
	UPROPERTY(VisibleAnywhere, Category = "RPG|Status")
	TMap<TObjectPtr<UStatus>, FGameplayTag> ObjTagMap;
	TSet<TObjectPtr<UStatus>> TickedStatuses;

protected:

public:

	URPGComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "RPG")
	void TurnStart();

	UFUNCTION(BlueprintCallable, Category = "RPG")
	void TurnEnd();

	UFUNCTION(BlueprintCallable, Category="RPG|Status")
	void ApplyStatus(UStatus* Status, int Stacks=1);

	UFUNCTION(BlueprintCallable, Category = "RPG|Status")
	void RemoveStatus(FGameplayTag StatusID, bool bAllInstances=true);

	UFUNCTION(BlueprintCallable, Category = "RPG|Status")
	void RemoveStatusInstance(UStatus* Status);

	UFUNCTION(BlueprintCallable, Category = "RPG|Status", meta=(DeterminesOutputType="Status"))
	UStatus* MakeStatus(TSubclassOf<UStatus> Status, FGameplayTag StatusID);

	template <class T>
	T* MakeStatus(FGameplayTag StatusID)
	{
		auto Status = NewObject<T>(this, T::StaticClass());
		Status->StatusID = StatusID;
		return Status;
	}

	UFUNCTION(BlueprintCallable, Category = "RPG|Status")
	void GetStatus(FGameplayTag StatusID, UPARAM(Ref) TArray<UStatus*>& Found);

	UFUNCTION(BlueprintCallable, Category = "RPG|Status")
	URPGProperty* FindRPGPropertyByName(FName Ref) const;

	UFUNCTION(BlueprintCallable, Category = "RPG|Status")
	void PauseStatus();

	UFUNCTION(BlueprintCallable, Category = "RPG|Status")
	void UnpauseStatus();

	TArray<FString> GetRPGPropertyNames(TSubclassOf<URPGProperty> Props) const;

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

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	UFUNCTION()
	void OnStatusExpired(UStatus* Status);
};