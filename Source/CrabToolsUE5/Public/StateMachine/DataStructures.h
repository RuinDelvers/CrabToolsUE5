#pragma once

#include "CoreMinimal.h"
#include "DataStructures.generated.h"

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

	UPROPERTY(EditAnywhere, Category = "StateMachine|Events",
		meta = (GetOptions = "GetEventOptions", EditCondition="!bIsDefined", EditConditionHides, AllowPrivateAccess))
	FName InlinedEventName;

	UPROPERTY(EditAnywhere, Category = "StateMachine|Events",
		meta = (GetOptions = "GetEventOptions", EditCondition = "bIsDefined", EditConditionHides, AllowPrivateAccess))
	FName DefinedEventName;

	UPROPERTY(EditDefaultsOnly, Category = "StateMacine|Events", meta=(AllowPrivateAccess))
	bool bIsDefined = false;

	#if WITH_EDITORONLY_DATA
		UPROPERTY(EditDefaultsOnly, Category="StateMacine|Events", meta=(AllowPrivateAccess))
		bool bIsEmitted = false;
	#endif

public:

	operator const FName&() const
	{
		if (this->bIsDefined)
		{
			return this->DefinedEventName;
		}
		else
		{
			return this->InlinedEventName;
		}
	}

	bool IsNone() const
	{
		if (this->bIsDefined)
		{
			return this->DefinedEventName.IsNone();
		}
		else
		{
			return this->InlinedEventName.IsNone();
		}
	}

	const FName& GetEvent() const
	{
		if (this->bIsDefined)
		{
			return this->DefinedEventName;
		}
		else
		{
			return this->InlinedEventName;
		}
	}

	void SetEvent(const FName& NewEvent)
	{
		if (this->bIsDefined)
		{
			this->DefinedEventName = NewEvent;
		}
		else
		{
			this->InlinedEventName = NewEvent;
		}
	}

	friend uint32 GetTypeHash(const FEventSlot& Value)
	{
		if (Value.bIsDefined)
		{
			return GetTypeHash(Value.DefinedEventName);
		}
		else
		{
			return GetTypeHash(Value.InlinedEventName);
		}		
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

/* Structure used to store a reference to a submachine. */
USTRUCT(BlueprintType)
struct FSubMachineSlot
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "StateMachine",
		meta = (GetOptions = "GetMachineOptions"))
	FName MachineName;

	friend uint32 GetTypeHash(const FSubMachineSlot& Value)
	{
		return GetTypeHash(Value.MachineName);
	}
};