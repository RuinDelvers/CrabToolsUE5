#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "StateMachine/DataStructures.h"
#include "EventTriggerBox.generated.h"

UCLASS(Blueprintable, Category = "StateMachine")
class CRABTOOLSUE5_API AEventTriggerBox : public ATriggerBox
{
	GENERATED_BODY()

	/* The event emitted to an overlapped actor. */
	UPROPERTY(EditAnywhere, Category = "StateMachine|Events",
		meta = (AllowPrivateAccess, GetOptions = "GetEventOptions"))
	FEventSlot BeginEvent;

	/* The event emitted to an overlapped actor that no longer is overlapping. */
	UPROPERTY(EditAnywhere, Category = "StateMachine|Events")
	FEventSlot EndEvent;

	/* Whether or not to use data events vs basic events. */
	UPROPERTY(EditAnywhere, Category = "StateMachine|Events")
	bool bUseData = false;

	UPROPERTY(EditAnywhere, Category = "StateMachine|Events",
		meta = (AllowPrivateAccess, EditCondition = "bUseData", EditConditionHides))
	TSoftObjectPtr<UObject> Data;

public:

	AEventTriggerBox();

protected:

	virtual void BeginPlay() override;

	#if WITH_EDITOR
		virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	#endif // WITH_EDITOR

protected:

	UFUNCTION()
	void OnBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);
};