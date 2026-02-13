#pragma once

#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "AIStructs.generated.h"


USTRUCT(BlueprintType)
struct FMoveToData
{
	GENERATED_USTRUCT_BODY()

public:

	FNavPathSharedPtr SavedPath;

	/* If this is true, then controllers using this will attempt to */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "StateMachine|AI")
	bool bResumePreviousPath = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "StateMachine|AI")
	bool bUseOverrideLocation = false;

	/* If this is true, and there is no goal actor, it'll always default to that location. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "StateMachine|AI")
	bool bUseLocationIfNoGoal = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "StateMachine|AI",
		meta=(MakeEditWidget))
	FVector DestinationLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "StateMachine|AI")
	TObjectPtr<AActor> DestinationActor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "StateMachine|AI")
	bool bUsePathfinding = true;

	FPathFollowingRequestResult Result;

public:


	void ResetGoal();
	void ClearIfNoCache();
	bool ResumeMove(AAIController* Ctrl) const;
	bool PauseMove(AAIController* Ctrl) const;
	void MakeRequest(AAIController* Ctrl);
};