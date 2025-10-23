#pragma once

#include "StateMachine/Transitions/BaseTransitions.h"
#include "EventComboTransition.generated.h"

UENUM()
enum class EEventComboType
{
	INFINITE UMETA(DisplayName = "Infinite"),
	FRAME UMETA(DisplayName = "Frame"),
	TIME UMETA(DisplayName = "Time"),
	ON_EXIT UMETA(DisplayName = "On Exit"),
};

USTRUCT(BlueprintType)
struct FEventComboNodeDataPoint
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Events")
	EEventComboType Type = EEventComboType::ON_EXIT;

	UPROPERTY(VisibleAnywhere, Category = "Events")
	bool bActive = false;

	UPROPERTY(EditAnywhere, Category = "Events",
		meta = (EditCondition = "Type==EEventComboType::TIME",
			EditConditionHides))
	float Time = 0.0;

	FTimerHandle TimeUp;

public:

	void Activate();
	void Deactivate();

private:

	void TimeUpCallback();
};

USTRUCT(BlueprintType)
struct FEventComboNodeData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Events",
		meta = (GetKeyOptions = "GetEventOptions"))
	TMap<FName, FEventComboNodeDataPoint> Events;

public:

	bool Activate(FName EName);
	bool Deactivate(FName EName);
	bool IsActive() const;
	void Reset();
	void Exit();
};

/**
 *
 */
UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced, CollapseCategories, DisplayName = "EventCombo")
class CRABTOOLSUE5_API UEventComboTransitionCondition : public UTransitionCondition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Transition")
	TArray<FEventComboNodeData> Combos;

public:

	virtual bool Check() const override;
	virtual void Exit_Implementation() override;
	virtual void Event_Implementation(FName InEvent) override;

private:
	#if WITH_EDITOR
		UFUNCTION()
		TArray<FString> GetEventOptions() const;
	#endif
};

/**
 *
 */
UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced, CollapseCategories, DisplayName = "EventCombo")
class CRABTOOLSUE5_API UEventComboTransitionDataCondition : public UTransitionDataCondition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Events")
	TArray<FEventComboNodeData> Combos;

public:

	virtual bool Check(UObject* Data) const override;
	virtual void Exit_Implementation() override;
	virtual void Event_Implementation(FName InEvent) override;

private:
	#if WITH_EDITOR
		UFUNCTION()
		TArray<FString> GetEventOptions() const;
	#endif
};