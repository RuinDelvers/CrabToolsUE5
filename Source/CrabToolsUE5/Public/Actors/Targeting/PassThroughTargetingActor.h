#pragma once

#include "CoreMinimal.h"
#include "Actors/Targeting/BaseTraceTargetingActor.h"
#include "PassThroughTargetingActor.generated.h"

/*
 * This targeting actor simply utilizes the ITargeterInterface and directly passes them through.
 * This is primarily used for a "trace" targeter that doesn't care about collision.
 */
UCLASS(Blueprintable)
class APassThroughTargetingActor : public ABaseTraceTargetingActor
{
	GENERATED_BODY()

public:

	virtual void Tick(float DeltaTime) override;
};
