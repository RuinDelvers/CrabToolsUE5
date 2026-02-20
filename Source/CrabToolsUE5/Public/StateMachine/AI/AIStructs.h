#pragma once

#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "AIStructs.generated.h"

class UNavigationQueryFilter;
class UNavigationPath;

USTRUCT(BlueprintType)
struct FMoveToData
{
	GENERATED_USTRUCT_BODY()

public:

	FNavPathSharedPtr SavedPath;

	bool bUseOverrideLocation = false;

	/* If this is true, and there is no goal actor, it'll always default to that location. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Pathing")
	bool bUseLocationIfNoGoal = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Pathing",
		meta=(MakeEditWidget))
	FVector DestinationLocation = FVector::ZeroVector;
	FVector OverrideDestinationLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Pathing")
	TObjectPtr<AActor> DestinationActor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Pathing")
	bool bUsePathfinding = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Pathing")
	bool bAllowPartialPaths = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Pathing")
	TSubclassOf<UNavigationQueryFilter> FilterClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Pathing")
	float AcceptanceRadius = -1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Pathing")
	bool bStopOnOverlap = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Pathing")
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

	/*
	 * Makes a request utilizing this data, but forcing the use of a specific path.
	 * Returns true if the requst was successful, false otherwise.
	*/
	bool MakeRequest(AAIController* Ctrl, UNavigationPath* Path);

	FAIMoveRequest ConstructRequest(AAIController* Ctrl) const;
};