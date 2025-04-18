#include "Actors/Targeting/PassThroughTargetingActor.h"
#include "Actors/Targeting/ITargeter.h"

void APassThroughTargetingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	auto Target = ITargeterInterface::Execute_GetTracedActor(this->GetUsingActorNative());
	auto EndPoint = ITargeterInterface::Execute_GetEndPoint(this->GetUsingActorNative());

	this->UpdateTraces(Target, EndPoint);
}
