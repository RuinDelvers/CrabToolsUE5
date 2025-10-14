#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/StateMachineComponent.h"
#include "StateMachine/EventListener.h"
#include "SMCharacter.generated.h"


UCLASS(Blueprintable)
class ASMCharacter : public ACharacter, public IEventListenerInterface
{
	GENERATED_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StateMachine",
		meta=(AllowPrivateAccess=true))
	class UStateMachineComponent* StateMachineComponent;

public:
	ASMCharacter();

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

