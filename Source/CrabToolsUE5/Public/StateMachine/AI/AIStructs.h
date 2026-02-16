#pragma once

#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "AIStructs.generated.h"

class UNavigationQueryFilter;

USTRUCT(BlueprintType)
struct FMoveToData
{
	GENERATED_USTRUCT_BODY()

public:

	FNavPathSharedPtr SavedPath;

	bool bUseOverrideLocation = false;

	/* If this is true, and there is no goal actor, it'll always default to that location. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "StateMachine|AI")
	bool bUseLocationIfNoGoal = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "StateMachine|AI",
		meta=(MakeEditWidget))
	FVector DestinationLocation = FVector::ZeroVector;
	FVector OverrideDestinationLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "StateMachine|AI")
	TObjectPtr<AActor> DestinationActor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "StateMachine|AI")
	bool bUsePathfinding = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "StateMachine|AI")
	bool bAllowPartialPaths = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "StateMachine|AI")
	TSubclassOf<UNavigationQueryFilter> FilterClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "StateMachine|AI")
	float AcceptanceRadius = -1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "StateMachine|AI")
	bool bStopOnOverlap = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "StateMachine|AI")
	bool bCanStrafe = true;

	FPathFollowingRequestResult Result;

public:

	void SetOverrideLocation(FVector NewLocation);

	void ResetGoal();
	bool ResumeMove(AAIController* Ctrl) const;
	bool PauseMove(AAIController* Ctrl) const;

	/*
	 * Makes the movement request to the given AI controller. Assumes non-null.
	 * Will return true if was able to make a request, regardless of whether or not that request was successful or not.
	 */
	bool MakeRequest(AAIController* Ctrl);
};