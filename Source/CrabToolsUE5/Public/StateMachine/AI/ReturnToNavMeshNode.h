#pragma once

#include "StateMachine/AI/BaseNode.h"
#include "Navigation/PathFollowingComponent.h"
#include "ReturnToNavMeshNode.generated.h"

/**
 * Simple node for making an entity follow a PatrolPath actor's path.
 */
UCLASS(Blueprintable, Category = "StateMachine|AI")
class CRABTOOLSUE5_API UReturnToNavMeshNode : public UAIBaseNode
{
	GENERATED_BODY()

	/* The radius to search for a point on the navmesh to return to. */
	UPROPERTY(EditAnywhere, Category = "Navigation")
	float Radius = 50;

public:

	UReturnToNavMeshNode();

	virtual void Enter_Inner_Implementation() override;

private:

	UFUNCTION()
	void OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);

	void BindCallback();
	void UnbindCallback();
};
