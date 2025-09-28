#include "Actors/Targeting/PassThroughTargetingActor.h"
#include "Actors/Targeting/ITargeter.h"

void APassThroughTargetingActor::HandleTrace_Implementation()
{
	Super::HandleTrace_Implementation();

	auto Target = ITargeterInterface::Execute_GetTracedActor(this->GetUsingActorNative());
	auto EndPoint = ITargeterInterface::Execute_GetEndPoint(this->GetUsingActorNative());

	FTargetingData InData;

	InData.TargetActor = Target;
	InData.TargetLocation = EndPoint;

	this->UpdateTraces(InData);
}
