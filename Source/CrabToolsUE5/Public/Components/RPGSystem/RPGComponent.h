#pragma once

#include <limits>
#include "Templates/SharedPointer.h"
#include "Utils/Enums.h"
#include "RPGComponent.generated.h"

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
	void Turn();
	virtual void Turn_Implementation() {}

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


#pragma region Integer Attributes


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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIntAttributeObserver, int, Value);
DECLARE_DYNAMIC_DELEGATE_OneParam(FIntAttributeCallback, int, Value);

USTRUCT(BlueprintType, meta = (DisableSplitPin))
struct CRABTOOLSUE5_API FIntAttribute 
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="RPG|Attributes", meta=(AllowPrivateAccess=true))
	int BaseValue = 0;
	int CompValue;

	URPGComponent* Owner;

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "RPG|Attributes", meta = (AllowPrivateAccess = true))
	TArray<UIntOperator*> Operators;

	TArray<FIntResource*> Dependencies;

public:
	FIntAttributeObserver ValueChangedEvent;

	int GetValue() const;
	void Operate(UIntOperator* Op);
	void UnOperate(UIntOperator* Op);
	void SetOwner(URPGComponent* UOwner);
	void Initialize(URPGComponent* UOwner);
	void Refresh();
	void AddDependency(FIntResource* Dep);
};

USTRUCT(BlueprintType, meta = (DisableSplitPin))
struct CRABTOOLSUE5_API FIntResource
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RPG|Attributes", meta = (AllowPrivateAccess = true))
	int Value = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RPG|Attributes", meta = (AllowPrivateAccess = true, GetOptions = "GetIntAttributeNames"))
	FName MinAttribute;
	FIntAttribute* MinAttributeRef;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RPG|Attributes", meta = (AllowPrivateAccess = true, GetOptions="GetIntAttributeNames"))
	FName MaxAttribute;
	FIntAttribute* MaxAttributeRef;

	URPGComponent* Owner;


public:

	FIntAttributeObserver ValueChangedEvent;

	int GetValue() const { return this->Value; }
	void SetValue(int UValue);
	void SetOwner(URPGComponent* UOwner);
	void Initialize(URPGComponent* UOwner);
	void Refresh();

	int GetMax() const;
	int GetMin() const;
	float GetPercent() const;
};

#pragma endregion

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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFloatAttributeObserver, float, Value);
DECLARE_DYNAMIC_DELEGATE_OneParam(FFloatAttributeCallback, float, Value);

USTRUCT(BlueprintType, meta = (DisableSplitPin))
struct CRABTOOLSUE5_API FFloatAttribute
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RPG|Attributes", meta = (AllowPrivateAccess = true))
	float BaseValue = 0;
	float CompValue;

	URPGComponent* Owner;

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "RPG|Attributes", meta = (AllowPrivateAccess = true))
	TArray<UFloatOperator*> Operators;

	TArray<FFloatResource*> Dependencies;

public:
	FFloatAttributeObserver ValueChangedEvent;

	float GetValue() const;
	void Operate(UFloatOperator* Op);
	void UnOperate(UFloatOperator* Op);
	void SetOwner(URPGComponent* UOwner);
	void Initialize(URPGComponent* UOwner);
	void Refresh();
	void AddDependency(FFloatResource* Dep);
};

USTRUCT(BlueprintType, meta=(DisableSplitPin))
struct CRABTOOLSUE5_API FFloatResource
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RPG|Attributes", meta = (AllowPrivateAccess = true))
	float Value = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RPG|Attributes", meta = (AllowPrivateAccess = true, GetOptions = "GetFloatAttributeNames"))
	FName MinAttribute;
	FFloatAttribute* MinAttributeRef;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RPG|Attributes", meta = (AllowPrivateAccess = true, GetOptions = "GetFloatAttributeNames"))
	FName MaxAttribute;
	FFloatAttribute* MaxAttributeRef;

	URPGComponent* Owner;


public:
	FFloatAttributeObserver ValueChangedEvent;

	float GetValue() const { return this->Value; }
	void SetValue(float UValue);
	void SetOwner(URPGComponent* UOwner);
	void Initialize(URPGComponent* UOwner);
	void Refresh();

	float GetMax() const;
	float GetMin() const;
	float GetPercent() const;
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

	UPROPERTY(BlueprintReadOnly, Category = "RPG", meta=(AllowPrivateAccess=true))
	FIntAttribute ZeroInt;

	UPROPERTY(BlueprintReadOnly, Category = "RPG", meta = (AllowPrivateAccess = true))
	FFloatAttribute ZeroFloat;

public:

	URPGComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "RPG")
	void Turn();

	UFUNCTION(BlueprintCallable, Category="RPG|Status")
	void ApplyStatus(UStatus* Status);

	UFUNCTION(BlueprintCallable, Category = "RPG|Status")
	void StackStatus(TSubclassOf<UStatus> StatusType, int Quantity=1);

	UFUNCTION(BlueprintCallable, Category = "RPG|Status")
	void RemoveStatus(UStatus* Status);

	UFUNCTION(BlueprintCallable, Category = "RPG|Status", meta=(ExpandEnumAsExecs="Result", DeterminesOutputType="SClass"))
	UStatus* GetStatus(TSubclassOf<UStatus> SClass, ESearchResult& Result);


protected:

	virtual void InitializeComponent() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	#if WITH_EDITOR
		virtual void PostEditChangeProperty(struct FPropertyChangedEvent& e) override;
	#endif

	virtual void PostLoad() override;
	void Validate();

private:

	UFUNCTION()
	TArray<FString> GetIntAttributeNames() const;

	UFUNCTION()
	TArray<FString> GetFloatAttributeNames() const;
};