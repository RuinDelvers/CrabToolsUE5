#pragma once

#include "Utils/Enums.h"
#include "Ability/Ability.h"
#include "AbilityChain.generated.h"

/**
 * Class for player abilities. Abilities consist of a start, a continuous action,
 * a discrete action that happens on request, and an finish.
 */
UCLASS(Blueprintable)
class CRABTOOLSUE5_API UAbilityChain : public UAbility
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, Instanced, EditDefaultsOnly, Category="Ability",
		meta=(AllowPrivateAccess))
	TArray<TObjectPtr<UAbility>> AbilityChain;

	int ActiveIndex = 0;


public:

	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category="Ability",
		meta=(DeterminesOutputType="Type", ExpandEnumAsExecs="Result"))
	UAbility* GetChild(TSubclassOf<UAbility> Type, int Index, ESearchResult& Result) const;

protected:

	virtual void Initialize_Inner_Implementation() override;
	virtual void Start_Inner_Implementation() override;
	virtual void Perform_Inner_Implementation() override;
	virtual void Tick_Inner_Implementation(float DeltaTime) override;
	virtual void Finish_Inner_Implementation() override;
	virtual bool RequiresTick_Implementation() const override;
	virtual void Detach_Inner_Implementation() override;

private:

	UFUNCTION()
	void HandleFinish(UAbility* Abi);
};
