#pragma once

#include "CoreMinimal.h"
#include "Delegates/DelegateSignatureImpl.inl"
#include "Ability/Ability.h"
#include "AnimAbility.generated.h"

/**
 * Class for player abilities. Abilities consist of a start, a continuous action,
 * a discrete action that happens on request, and an finish.
 * 
 * This node will listen for an AnimNotify_AbilityFinish. You can implement this event either on a per
 * animation basis, or as a notify in the AnimBP on a transition. E.g. setup automatic transitions for
 * an ability state, then the notify will be the transition away. That way the AnimBP drives the
 * ability. This second method saves work by not requiring a notify in every animation asset.
 */
UCLASS(Blueprintable)
class CRABTOOLSUE5_API UAnimAbility : public UAbility
{
	GENERATED_BODY()


protected:

	#if WITH_EDITORONLY_DATA
		UPROPERTY(EditDefaultsOnly, Category = "Animation", meta = (AllowPrivateAccess))
		TSubclassOf<AActor> ActorClass;
	#endif //WITH_EDITORONLY_DATA

	UPROPERTY(EditDefaultsOnly, Category = "Animation",
		meta = (AllowPrivateAccess, GetOptions = "GetComponentOptions"))
	FName ComponentName;

	UPROPERTY(Transient)
	TObjectPtr<USkeletalMeshComponent> Component;

	UPROPERTY(EditAnywhere, Category="Ability", meta=(GetOptions="GetCustomPhases"))
	TSet<FName> CustomPhases;

public:

	UAnimAbility(const FObjectInitializer& ObjectInitializer);

protected:

	void Initialize_Inner_Implementation(UObject* InitData) override;
	void Start_Inner_Implementation() override;
	void Finish_Inner_Implementation() override;
	
private:

	UFUNCTION()
	void AnimNotify_AbilityFinish();

	UFUNCTION()
	void AnimNotify_AbilityPerform();

	#if WITH_EDITOR
		void FilterPhases();

		UFUNCTION()
		TArray<FString> GetComponentOptions() const;

		UFUNCTION()
		TArray<FString> GetCustomPhases() const;

		virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
		virtual void PostLoad() override;
		virtual void PostLinkerChange() override;
	#endif //WITH_EDITOR
};
