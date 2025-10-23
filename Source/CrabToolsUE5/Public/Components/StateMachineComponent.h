#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StateMachine/StateMachine.h"
#include "Delegates/DelegateSignatureImpl.inl"
#include "Utils/Enums.h"
#include "StateMachine/EventListener.h"
#include "StateMachineComponent.generated.h"


UCLASS( ClassGroup=(General), meta=(BlueprintSpawnableComponent) )
class CRABTOOLSUE5_API UStateMachineComponent : public UActorComponent, public IEventListenerInterface
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "StateMachine", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UStateMachine> Machine;

public:	
	
	UStateMachineComponent();

protected:
	
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(
		float DeltaTime, 
		ELevelTick TickType, 
		FActorComponentTickFunction* ThisTickFunction)
		override;

	virtual void Event_Implementation(FName InEvent) override final { this->Event_Direct(InEvent); }
	void Event_Direct(FName InEvent);

	void EventWithData_Implementation(FName InEvent, UObject* Data) override final { this->EventWithData_Direct(InEvent, Data); }
	void EventWithData_Direct(FName InEvent, UObject* Data);

	UFUNCTION(BlueprintCallable, Category="StateMachine")
	void CreateMachine(TSubclassOf<UStateMachine> MachineClass);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateMachine")
	FName CurrentStateName();

	UStateMachine* GetMachine() const { return this->Machine; }

	#if WITH_EDITOR
		virtual void PostEditChangeProperty(FPropertyChangedEvent& Event) override;
	#endif

private:

	bool HasMachine();

	UFUNCTION()
	void TickUpdated(bool NeedsTick);
};
