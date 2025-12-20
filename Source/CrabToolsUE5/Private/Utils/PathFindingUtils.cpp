#include "Utils/PathFindingUtils.h"
#include "NavigationSystemTypes.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "Navigation/NavLinkProxy.h"
#include "Logging/MessageLog.h"

#define LOCTEXT_NAMESPACE "PathFindingUtils"

FVector UPathFindingUtilsLibrary::ChooseNearLocation(AAIController* Ctrl, TArray<FVector>& Points)
{
	UNavigationSystemV1* NavSys = Ctrl ? FNavigationSystem::GetCurrent<UNavigationSystemV1>(Ctrl->GetWorld()) : nullptr;

	if (Ctrl)
	{
		const auto AgentLocation = Ctrl->GetNavAgentLocation();
		const ANavigationData* NavData = NavSys->GetNavDataForProps(Ctrl->GetNavAgentPropertiesRef(), AgentLocation);
		
		FVector Dest = FVector::ZeroVector;
		bool bFoundPath = false;
		float PathLength = std::numeric_limits<float>::infinity();

		for (auto& Pt : Points)
		{
			FPathFindingQuery Query(Ctrl, *NavData, AgentLocation, Pt);
			FNavPathSharedPtr NavPath;

			auto Result = NavSys->FindPathSync(Query);

			if (Result.IsSuccessful())
			{
				bFoundPath = true;
				float Length = Result.Path->GetLength();

				if (Length < PathLength)
				{
					Dest = Pt;
					PathLength = Length;
				}
			}
		}

		if (bFoundPath) { return Dest; }
	}

	if (Points.Num() > 0)
	{
		return Points[0];
	}
	else
	{
		return FVector::Zero();
	}
}

FVector UPathFindingUtilsLibrary::NearestNavLinkPointToActor(ANavLinkProxy* NLP, AActor* Actor)
{
	#if WITH_EDITOR
		if (!IsValid(NLP) || !IsValid(Actor))
		{
			FMessageLog Log("PIE");
			Log.Error(LOCTEXT("Invalid Inputs", "Invalid Actors passed to NearestNavLinkPointToActor. This will crash in a packaged build."));
			return FVector::ZeroVector;
		}
	#endif

	FVector ActorLocation = Actor->GetTargetLocation();
	FVector Destination = Actor->GetActorLocation();
	float CheckDist = std::numeric_limits<float>::infinity();

	TArray<FNavigationLink> Links;
	TArray<FNavigationSegmentLink> Segments;
	NLP->GetNavigationLinksArray(Links, Segments);

	for (const auto& Link : Links)
	{
		float LeftDist = FVector::DistSquared(Link.Left, ActorLocation);
		float RightDist = FVector::DistSquared(Link.Right, ActorLocation);
		bool bLeftCloser = LeftDist < RightDist;
		float CloserDist = bLeftCloser ? LeftDist : RightDist;

		if (CloserDist < CheckDist)
		{
			CheckDist = CloserDist;
			Destination = bLeftCloser ? Link.Left : Link.Right;
		}
	}

	return Destination;
}

#undef LOCTEXT_NAMESPACE