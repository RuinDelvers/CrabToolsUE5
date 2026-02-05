#pragma once

#include "CoreMinimal.h"
#include "DataStructures.generated.h"

class UStateNode;
class UStateMachine;

USTRUCT(BlueprintType)
struct FEventSetRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category = "EventSet",
		meta = (MultiLine = true))
	FText Description;
};


/* Structure used to store an event to be used by a node. */
USTRUCT(BlueprintType)
struct FEventSlot
{
	GENERATED_USTRUCT_BODY()

private:

	UPROPERTY(EditAnywhere, Category = "StateMachine|Events")
	FName EventName;

public:

	FEventSlot() : EventName(NAME_None) {}
	FEventSlot(FName InName) : EventName(InName) {}

	FName Name() const
	{
		return this->EventName;
	}

	bool IsNone() const
	{
		return this->EventName.IsNone();
	}

	operator const FName() const
	{
		return this->EventName;
	}

	bool operator==(FEventSlot const& Other) const
	{
		return this->EventName == Other.EventName;
	}

	bool operator!=(FEventSlot const& Other) const
	{
		return this->EventName != Other.EventName;
	}

	friend uint32 GetTypeHash(const FEventSlot& Value)
	{
		return GetTypeHash(Value.EventName);
	}
};

USTRUCT(BlueprintType)
struct FStateSlot
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "StateMachine|Events",
		meta = (GetOptions = "GetStateOptions"))
	FName StateName;

	friend uint32 GetTypeHash(const FStateSlot& Value)
	{
		return GetTypeHash(Value.StateName);
	}
};

USTRUCT(BlueprintType)
struct FIncomingStateSlot
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "StateMachine|Events",
		meta = (GetOptions = "GetIncomingStateOptions"))
	FName StateName;

	friend uint32 GetTypeHash(const FIncomingStateSlot& Value)
	{
		return GetTypeHash(Value.StateName);
	}
};

USTRUCT(BlueprintType)
struct FOutgoingStateSlot
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "StateMachine|Events",
		meta = (GetOptions = "GetOutgoingStateOptions"))
	FName StateName;

	friend uint32 GetTypeHash(const FOutgoingStateSlot& Value)
	{
		return GetTypeHash(Value.StateName);
	}
};


UENUM(BlueprintType)
enum class EHierarchyInputType : uint8
{
	/* Use an event or state machine that is defined in the blueprint. */
	DEFINED       UMETA(DisplayName = "Defined"),
	/* Use an event or state machine that is inlined in this slot. */
	INLINED       UMETA(DisplayName = "Inlined"),
	/* Use a machine class to dynamically make a state machine*/
	CUSTOM        UMETA(DisplayName="Custom")
};

/*
 * This class is used to dynamically select state machine classes for custom hierarchy slots.
 * An example usage would be dynamic AIs used for a submachine, and instead of creating new 
 * State Machines for each AI slot, we could use this to return a class to dynamically instantiate
 * one to use.
 */
UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class UStateMachineClassGenerator : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="StateMachine")
	TSoftClassPtr<UStateMachine> GetMachineClass(UStateMachine* OwnerMachine) const;
	virtual TSoftClassPtr<UStateMachine> GetMachineClass_Implementation(UStateMachine* OwnerMachine) const { return nullptr; }
};

/* Structure used to store a reference to a submachine. */
USTRUCT(BlueprintType)
struct FSubMachineSlot
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category = "StateMachine")
	EHierarchyInputType StateMachineSource = EHierarchyInputType::DEFINED;

	UPROPERTY(EditAnywhere, Instanced, Category = "StateMachine",
		meta = (ShowInnerProperties, ShowOnlyInnerProperties,
			EditCondition = "StateMachineSource == EHierarchyInputType::INLINED", EditConditionHides))
	TObjectPtr<UStateMachine> SubMachine;

	UPROPERTY(EditAnywhere, Instanced, Category = "StateMachine",
		meta = (ShowInnerProperties, ShowOnlyInnerProperties,
			EditCondition = "StateMachineSource == EHierarchyInputType::CUSTOM", EditConditionHides))
	TObjectPtr<UStateMachineClassGenerator> MachineClass;


	UPROPERTY(EditAnywhere, Category = "StateMachine",
		meta = (GetOptions = "GetMachineOptions",
			EditCondition = "StateMachineSource == EHierarchyInputType::DEFINED",
			EditConditionHides))
	FName MachineName;

public:

	void Initialize(UStateMachine* MachineSource);

	operator UStateMachine* () const { return this->SubMachine; }
	operator bool() const { return this->SubMachine != nullptr; }
	UStateMachine* operator->() const { return this->SubMachine; }

	bool DoesReferenceMachine(FName InMachineName) const;

	friend uint32 GetTypeHash(const FSubMachineSlot& Value)
	{
		return GetTypeHash(Value.MachineName);
	}

	void Validate();
};