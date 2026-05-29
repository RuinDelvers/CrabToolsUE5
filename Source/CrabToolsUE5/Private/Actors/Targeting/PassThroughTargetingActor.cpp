#include "Actors/Targeting/PassThroughTargetingActor.h"
#include "Actors/Targeting/ITargeter.h"

void APassThroughTargetingActor::HandleTrace_Implementation(FTargetingData& OutData)
{
	auto Target = ITargeterInterface::Execute_GetTracedActor(this->GetUsingActorNative());
	auto EndPoint = ITargeterInterface::Execute_GetEndPoint(this->GetUsingActorNative());

	FTargetingData InData;

	OutData.TargetActor = Target;
	OutData.TargetLocation = EndPoint;
	OutData.SourcePoint = this->GetActorLocation();
}
