#pragma once

#include "Actors/Targeting/BaseTargetingActor.h"
#include "DistanceTargetFilter.generated.h"

UENUM()
enum class EDistanceTargetFilterLocationSource
{
	POINT UMETA(DisplayName="Traced Point"),
	TARGET_ACTOR UMETA(DisplayName = "Target Actor"),
};

UENUM()
enum class EDistanceSourceFilterLocationSource
{
	TARGETING UMETA(DisplayName="Targeting Actor"),
	USING_ACTOR UMETA(DisplayName = "Using Actor"),
};

UCLASS(Blueprintable, CollapseCategories, ClassGroup = (Custom),
	meta = (BlueprintSpawnableComponent))
class CRABTOOLSUE5_API UDistanceTargetFilterComponent : public UTargetFilterComponent
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Ability",
		meta=(AllowPrivateAccess, EditCondition="!bUseAbilityDistance", EditConditionHides))
	float Range = std::numeric_limits<float>::infinity();

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	bool bUseAbilityDistance = false;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	EDistanceTargetFilterLocationSource TargetPointSource = EDistanceTargetFilterLocationSource::POINT;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	EDistanceSourceFilterLocationSource SourcePointSource = EDistanceSourceFilterLocationSource::TARGETING;

	UPROPERTY(EditDefaultsOnly, Category = "Navigation")
	FText ErrorMessage;

public:

	UDistanceTargetFilterComponent();

	virtual void BeginPlay() override;
	virtual bool Filter_Implementation(const FTargetingData& Data, FText& FailureReason) override;


private:

	void MissingAbilityError() const;

	UFUNCTION()
	void InitFromAbility(UAbilityData* Data);
	
	FORCEINLINE FVector GetTargetPoint(const FTargetingData& Data) const;

	FORCEINLINE FVector GetSourccePoint() const;
};