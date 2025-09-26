#pragma once

#include "CoreMinimal.h"
#include "Actors/Targeting/BaseTraceTargetingActor.h"
#include "CollisionQueryParams.h"
#include "LineTraceTargetingActor.generated.h"

UCLASS(Blueprintable)
class ALineTraceTargetingActor : public ABaseTraceTargetingActor
{
	GENERATED_BODY()


public:

	virtual void Tick(float DeltaTime) override;
};
