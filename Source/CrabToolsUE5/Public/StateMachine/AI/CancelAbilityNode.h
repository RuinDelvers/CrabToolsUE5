#pragma once

#include "CoreMinimal.h"
#include "StateMachine/StateMachine.h"
#include "CancelAbilityNode.generated.h"

class UAbility;

/* Used to cancel abilities. Will take in Data for an ability or HasAbilityInterface,
 * or if the bCanCallOwner is set to true, will use the HasAbilityInterface on the owner.
 */
UCLASS(Blueprintable, Category = "StateMachine|AI")
class CRABTOOLSUE5_API UCancelAbilityNode : public UStateNode
{
	GENERATED_BODY()

	/* Whether or not we can use the HasAbilityInterface on the owner to find a cancelable ability. */
	UPROPERTY(EditDefaultsOnly, Category="StateMachine|AI")
	bool bCanCallOwner = true;

public:

	UCancelAbilityNode();

	virtual void Enter_Inner_Implementation() override;
	virtual void EnterWithData_Inner_Implementation(UObject* Data) override;
};
