#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/StateMachineComponent.h"
#include "SMPawn.generated.h"


UCLASS(Blueprintable)
class ASMPawn : public APawn, public IEventListenerInterface
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = StateMachine, 
		meta = (AllowPrivateAccess = "true"))
	class UStateMachineComponent* StateMachineComponent;

public:
	ASMPawn();


	virtual void Event_Implementation(FName InEvent) override final { this->Event_Direct(InEvent); }
	void Event_Direct(FName InEvent)
	{
		this->StateMachineComponent->Event_Direct(InEvent);
	}
	void EventWithData_Implementation(FName InEvent, UObject* Data) override final { this->EventWithData_Direct(InEvent, Data); }
	void EventWithData_Direct(FName InEvent, UObject* Data)
	{
		this->StateMachineComponent->EventWithData_Direct(InEvent, Data);
	}
};

