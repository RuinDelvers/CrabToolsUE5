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

	virtual void Event_Implementation(FName InEvent, UObject* Source) override final { this->Event_Direct(InEvent, Source); }
	void Event_Direct(FName InEvent, UObject* Source)
	{
		this->StateMachineComponent->Event_Direct(InEvent, Source);
	}
	void EventWithData_Implementation(FName InEvent, UObject* Data, UObject* Source) override final { this->EventWithData_Direct(InEvent, Data, Source); }
	void EventWithData_Direct(FName InEvent, UObject* Data, UObject* Source)
	{
		this->StateMachineComponent->EventWithData_Direct(InEvent, Data, Source);
	}
};

