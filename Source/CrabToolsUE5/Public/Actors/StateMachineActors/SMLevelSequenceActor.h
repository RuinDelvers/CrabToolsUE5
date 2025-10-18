#pragma once

#include "LevelSequenceActor.h"
#include "Components/StateMachineComponent.h"
#include "SMLevelSequenceActor.generated.h"


UCLASS(Blueprintable)
class ASMLevelSequenceActor : public ALevelSequenceActor, public IEventListenerInterface
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="StateMachine",
		meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UStateMachineComponent> StateMachineComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="StateMachine",
		meta=(ExposeOnSpawn=true, AllowPrivateAccess))
	TSubclassOf<UStateMachine> CutsceneClass;

public:
	ASMLevelSequenceActor(const FObjectInitializer& ObjectInit);

	virtual void BeginPlay() override;

	virtual void Event_Implementation(FName EName) override final { this->Event_Direct(EName); }
	void Event_Direct(FName EName)
	{
		this->StateMachineComponent->Event_Direct(EName);
	}
	void EventWithData_Implementation(FName EName, UObject* Data) override final { this->EventWithData_Direct(EName, Data); }
	void EventWithData_Direct(FName EName, UObject* Data)
	{
		this->StateMachineComponent->EventWithData_Direct(EName, Data);
	}
};

