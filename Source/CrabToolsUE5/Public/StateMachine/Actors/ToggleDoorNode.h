#pragma once

#include "StateMachine/StateMachine.h"
#include "Actors/DoorActor.h"
#include "ToggleDoorNode.generated.h"

class UDoorActor;

/*
 * A simple node to apply EDoorActorActions to its owner. 
 */
UCLASS(Blueprintable, Category = "StateMachine|AI")
class CRABTOOLSUE5_API UToggleDoorNode : public UStateNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Doors")
	EDoorActorActions EnterAction = EDoorActorActions::NONE;

	UPROPERTY(EditAnywhere, Category = "Doors")
	EDoorActorActions ExitAction = EDoorActorActions::NONE;

	TWeakObjectPtr<ADoorActor> Actor;

public:

	virtual void Initialize_Inner_Implementation() override;
	virtual void Enter_Inner_Implementation() override;
	virtual void Exit_Inner_Implementation() override;
};
