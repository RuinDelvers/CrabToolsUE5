#pragma once

#include "StateMachine/ArrayNode.h"
#include "EventComboNode.generated.h"

UENUM()
enum class EEventComboType
{
	INFINITE UMETA(DisplayName = "Infinite"),
	FRAME UMETA(DisplayName="Frame"),
	TIME UMETA(DisplayName = "Time"),
};

USTRUCT(BlueprintType)
struct FEventComboNodeDataPoint
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Events")
	EEventComboType Type;

	UPROPERTY(VisibleAnywhere, Category = "Events")
	bool bActive = false;

	UPROPERTY(EditAnywhere, Category = "Events",
		meta=(EditCondition="Type==EEventComboType::TIME",
			EditConditionHides))
	float Time = 0.0;

	FTimerHandle TimeUp;

public:

	void Activate();

private:

	void TimeUpCallback();
};

USTRUCT(BlueprintType)
struct FEventComboNodeData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Events",
		meta=(GetKeyOptions="GetEventComboOptions"))
	TMap<FName, FEventComboNodeDataPoint> Events;

	UPROPERTY(EditAnywhere, Category = "Events")
	FName EmittedEvent;

public:

	bool IsActive() const;
	void Reset();
};


/**
 * Node that keeps track of events, and depending on different combinations
 * of received events, another one will be emitted.
 */
UCLASS(Blueprintable, Category = "StateMachine")
class CRABTOOLSUE5_API UEventComboNode : public UArrayNode
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, Category="Events")
	TArray<FEventComboNodeData> Combos;

protected:

	virtual void Event_Inner_Implementation(FName Event) override;
	virtual void EventWithData_Inner_Implementation(FName EName, UObject* Data) override;

	#if WITH_EDITORONLY_DATA
		virtual void GetEmittedEvents(TSet<FName>& Events) const override;

		UFUNCTION()
		TArray<FString> GetEventComboOptions() const;
	#endif

	void ReceiveEvent(FName EName);
};
