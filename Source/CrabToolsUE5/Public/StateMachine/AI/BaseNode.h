#pragma once

#include "CoreMinimal.h"
#include "StateMachine/StateMachine.h"
#include "BaseNode.generated.h"

class AAIController;

/**
 * Simple node for making an entity follow a PatrolPath actor's path.
 */
UCLASS(Blueprintable, Category = "StateMachine|AI")
class CRABTOOLSUE5_API UAIBaseNode : public UStateNode
{
	GENERATED_BODY()

	TWeakObjectPtr<AAIController> AICtrl;

public:

	virtual void Initialize_Inner_Implementation() override;

	/* Return the AIController that was retrieved by the Pawn onwer of the state machine. */

	UFUNCTION(BlueprintCallable, Category="AI")
	FORCEINLINE AAIController* GetAIController() const { return this->AICtrl.Get(); }

private:


	UFUNCTION()
	void OnControllerUpdated(APawn* Pawn, AController* OldController, AController* NewController);
};
