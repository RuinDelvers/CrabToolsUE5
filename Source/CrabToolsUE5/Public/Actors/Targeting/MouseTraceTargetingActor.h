#pragma once

#include "CoreMinimal.h"
#include "Actors/Targeting/BaseTraceTargetingActor.h"
#include "MouseTraceTargetingActor.generated.h"

UCLASS(Blueprintable)
class AMouseTraceTargetingActor : public ABaseTraceTargetingActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MouseTrace", meta=(AllowPrivateAccess, ExposeOnSpawn=true))
	float TraceDistance = 1000;

public:

	virtual void HandleTrace_Implementation() override;
};
