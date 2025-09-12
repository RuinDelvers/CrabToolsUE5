#pragma once

#include "Actors/Targeting/BaseTargetingActor.h"
#include "NavigableTargetFilter.generated.h"

UCLASS(Blueprintable, CollapseCategories, ClassGroup = (Custom),
	meta = (BlueprintSpawnableComponent))
class CRABTOOLSUE5_API UNavigableTargetFilterComponent : public UTargetFilterComponent
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category="Navigation")
	FVector QueryExtent = FVector(10, 10, 10);

public:

	virtual bool Filter_Implementation(const FTargetingData& Data, FText& FailureReason) override;

};