#pragma once

#include "Actors/Targeting/BaseTargetingActor.h"
#include "DistanceTargetFilter.generated.h"

UCLASS(Blueprintable, CollapseCategories, ClassGroup = (Custom),
	meta = (BlueprintSpawnableComponent))
class CRABTOOLSUE5_API UDistanceTargetFilterComponent : public UTargetFilterComponent
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Navigation",
		meta=(AllowPrivateAccess, EditCondition="!bUseAbilityDistance", EditConditionHides))
	float Range;

	UPROPERTY(EditDefaultsOnly, Category = "Navigation")
	bool bUseAbilityDistance = false;

public:

	UDistanceTargetFilterComponent();

	virtual void BeginPlay() override;
	virtual bool Filter_Implementation(const FTargetingData& Data, FText& FailureReason) override;


private:

	void MissingAbilityError() const;

	UFUNCTION()
	void InitFromAbility(UAbilityData* Data);
	
};