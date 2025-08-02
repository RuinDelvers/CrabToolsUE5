#pragma once

#include <limits>
#include "Utils/Enums.h"
#include "RPGComponent.generated.h"

class URPGProperty;

/* Base class for all Status objects for the RPG System*/
UCLASS(Blueprintable, DefaultToInstanced, CollapseCategories, EditInlineNew)
class UStatus: public UObject
{
	GENERATED_BODY()

	TWeakObjectPtr<URPGComponent> Owner;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="RPG|Status", meta=(AllowPrivateAccess, ExposeOnSpawn=true))
	float Duration = std::numeric_limits<float>::infinity();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG|Status", meta = (AllowPrivateAccess))
	bool bRefreshOnStack = true;

	int Stacks = 0;
	FTimerHandle Timer;

public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RPG|Status")
	bool IsPermanent() const { return this->Duration == std::numeric_limits<float>::infinity(); }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RPG|Status")
	bool IsOwned() const { return this->Owner.IsValid(); }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="RPG|Status")
	int GetStacks() const { return this->Stacks; }

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

	UFUNCTION(BlueprintCallable, Category="RPG|Status")
	void Apply(URPGComponent* Comp);

	UFUNCTION(BlueprintCallable, Category = "RPG|Status")
	void Stack(int Quantity = 1);

	UFUNCTION(BlueprintCallable, Category = "RPG|Status")
	void Detach();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RPG|Status")
	URPGComponent* GetOwner() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "RPG|Status")
	bool RequiresTick() const;
	virtual bool RequiresTick_Implementation() const { return false; }

	virtual UWorld* GetWorld() const override;

protected:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="RPG|Status")
	void Apply_Inner();
	virtual void Apply_Inner_Implementation() {}
	

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "RPG|Status")
	void Remove_Inner();
	virtual void Remove_Inner_Implementation() {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "RPG|Status")
	void Stack_Inner(int Quantity = 1);
	virtual void Stack_Inner_Implementation(int Quantity = 1) {}

	UFUNCTION(BlueprintCallable, Category="RPG|Status")
	void SetTimer();

private:

	UFUNCTION()
	void OnDurationExpired();

	friend class URPGComponent;

	void Remove();
};

UCLASS(Blueprintable, DefaultToInstanced, CollapseCategories, EditInlineNew)
class CRABTOOLSUE5_API UIntOperator: public UObject
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	int Priority;

	URPGComponent* Owner;

public:

	UFUNCTION(BlueprintNativeEvent, Category = "RPG|Operators")
	int Operate(int Value);
	virtual int Operate_Implementation(int Value) { return Value; }

	UFUNCTION(BlueprintNativeEvent, Category = "RPG|Operators")
	void Initialize();
	virtual void Initialize_Implementation() {  }


	int GetPriority() const { return this->Priority; }

	void SetOwner(URPGComponent* UOwner) { this->Owner = UOwner; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="RPG|Operators")
	URPGComponent* GetOwner() const { return this->Owner; }
};

#pragma region Float Attributes & Resources


UCLASS(Blueprintable, DefaultToInstanced, CollapseCategories, EditInlineNew)
class CRABTOOLSUE5_API UFloatOperator : public UObject
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	int Priority;

	URPGComponent* Owner;

public:

	UFUNCTION(BlueprintNativeEvent, Category = "RPG|Operators")
	float Operate(float Value);
	virtual float Operate_Implementation(float Value) { return Value; }

	UFUNCTION(BlueprintNativeEvent, Category = "RPG|Operators")
	void Initialize();
	virtual void Initialize_Implementation() {  }


	int GetPriority() const { return this->Priority; }

	void SetOwner(URPGComponent* UOwner) { this->Owner = UOwner; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RPG|Operators")
	URPGComponent* GetOwner() const { return this->Owner; }
};

#pragma endregion

/* Component that handles control, access, and manipualtion of Resources and Attributes in an RPG Setting.*/
UCLASS(Blueprintable, CollapseCategories, ClassGroup=(Custom),
	meta=(BlueprintSpawnableComponent))
class CRABTOOLSUE5_API URPGComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Instanced, Category="RPG|Status")
	TSet<TObjectPtr<UStatus>> Statuses;
	TSet<TObjectPtr<UStatus>> TickedStatuses;

public:

	URPGComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "RPG")
	void TurnStart();

	UFUNCTION(BlueprintCallable, Category = "RPG")
	void TurnEnd();

	UFUNCTION(BlueprintCallable, Category="RPG|Status")
	void ApplyStatus(UStatus* Status);

	UFUNCTION(BlueprintCallable, Category = "RPG|Status")
	void StackStatus(TSubclassOf<UStatus> StatusType, int Quantity=1);

	UFUNCTION(BlueprintCallable, Category = "RPG|Status")
	void RemoveStatus(UStatus* Status);

	UFUNCTION(BlueprintCallable, Category = "RPG|Status", meta=(ExpandEnumAsExecs="Result", DeterminesOutputType="SClass"))
	UStatus* GetStatus(TSubclassOf<UStatus> SClass, ESearchResult& Result);

	UFUNCTION(BlueprintCallable, Category = "RPG|Status")
	URPGProperty* FindRPGPropertyByName(FName Ref) const;

	TArray<FString> GetRPGPropertyNames(TSubclassOf<URPGProperty> Props) const;

protected:

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};