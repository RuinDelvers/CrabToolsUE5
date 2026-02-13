#pragma once

#include "StateMachine/AI/BaseNode.h"
#include "MovementRouterNode.generated.h"

UENUM(meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EAIMovementRouterType: uint8
{
	NONE   = 1 << 0 UMETA(DisplayName="None"),
	ENTER  = 1 << 2 UMETA(DisplayName = "Enter"),
	EXIT   = 1 << 3 UMETA(DisplayName = "Exit"),
	NOTIFY = 1 << 4 UMETA(DisplayName = "Notify"),
};

/**
 * Node which is used to make decisions about the type of movement to be made. Can take in a movement routing data
 * object as Enter data to decide, or can be extended to customize decision making.
 */
UCLASS(Blueprintable, Category = "StateMachine|AI")
class CRABTOOLSUE5_API UAIMovementRouterNode : public UAIBaseNode
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, Category="Movement", meta=(BitMask, BitMaskEnum="EAIMovementRouterType"))
	int FunctionFlags = 0;

public:

	UAIMovementRouterNode();

protected:

	virtual void Enter_Inner_Implementation() override;
	virtual void EnterWithData_Inner_Implementation(UObject* Data) override;
	virtual void Exit_Inner_Implementation() override;
	virtual void ExitWithData_Inner_Implementation(UObject* Data) override;
	
};