#pragma once

#include "Actors/Targeting/BaseTargetingActor.h"
#include "UniqueTargetFilter.generated.h"


UCLASS(Blueprintable, CollapseCategories, ClassGroup = (Custom),
	meta = (BlueprintSpawnableComponent))
class CRABTOOLSUE5_API UUniqueTargetFilterComponent : public UTargetFilterComponent
{
	GENERATED_BODY()

	TArray<FTargetingData> DataCache;

	UPROPERTY(EditDefaultsOnly, Category="Error")
	FText ErrorText;

public:

	UUniqueTargetFilterComponent();
	virtual bool Filter_Implementation(const FTargetingData& Data, FText& FailureReason) override;

};