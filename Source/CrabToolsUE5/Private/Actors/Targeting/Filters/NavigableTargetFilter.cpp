#include "Actors/Targeting/Filters/NavigableTargetFilter.h"
#include "NavigationSystem.h"

#define LOCTEXT_NAMESPACE "NavigableTargetFilter"

bool UNavigableTargetFilterComponent::Filter_Implementation(const FTargetingData& Data, FText& FailureReason)
{
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	FNavLocation NavLoc;

	auto Result = NavSys->ProjectPointToNavigation(Data.TargetLocation, NavLoc, QueryExtent);

	if (!Result)
	{
		FailureReason = LOCTEXT("ProjectionFailedResult", "Cannot navigate to location.");
	}

	return Result;
}

#undef LOCTEXT_NAMESPACE