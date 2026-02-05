#pragma once

#include "StateMachine/AI/BaseNode.h"
#include "Actors/Paths/PatrolPath.h"
#include "Navigation/PathFollowingComponent.h"
#include "Components/Pathing/PatrolPathFollowingComponent.h"
#include "StateMachine/DataStructures.h"
#include "Utils/ActorComponentPattern.h"
#include "SimplePatrolNode.generated.h"

/**
 * Simple node for making an entity follow a PatrolPath actor's path.
 */
UCLASS(Blueprintable, Category = "StateMachine|AI")
class CRABTOOLSUE5_API UAISimplePatrolNode : public UAIBaseNode
{
	GENERATED_BODY()

	/* 
	 * This value is used to guard against too many close patrol points. This is needed
	 * since "OnMoveCompleted" will be called if the next patrol point is right next to
	 * the current one, often leading to a StackOverflow.
	*/
	int RecurseGuard = 0;

	UPROPERTY(VisibleAnywhere, Category="AI", meta=(ShowInnerProperties))
	TObjectPtr<UActorComponentPatternRef> ComponentPattern;

	UPROPERTY(EditAnywhere, Category = "AI", meta=(AllowPrivateAccess))
	FName PathKey;

	UPROPERTY()
	mutable TObjectPtr<UPatrolPathFollowingComponent> CachedComp;
	
	UPROPERTY(EditAnywhere, Category = "AI")
	bool bCacheComponent = true;

	/* Previous states which would not reset the patrolling state. */
	UPROPERTY(EditDefaultsOnly, Category = "StateMachine|AI", meta=(ShowOnlyInnerProperties))
	TSet<FIncomingStateSlot> NonResetStates;

	FPathFollowingRequestResult Result;

public:
	UAISimplePatrolNode();

	virtual void Enter_Inner_Implementation() override;
	virtual void Exit_Inner_Implementation() override;
	virtual void Initialize_Inner_Implementation() override;
	virtual void SetActive_Inner_Implementation(bool bNewActive) override;
	
	UFUNCTION()
	void OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type MoveResult);

	#if WITH_EDITOR
		virtual void PostLinkerChange() override;

		UFUNCTION()
		TArray<FString> GetResetStateOptions() const;
	#endif

private:

	UPatrolPathFollowingComponent* GetPath() const;
	FPatrolPathState& GetState() const;

	void MoveToNext();
	void BindCallback();
	void UnbindCallback();
};
