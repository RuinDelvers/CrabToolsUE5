#pragma once

#include "StateMachine/Transitions/BaseTransitions.h"
#include "FunctionCallTransition.generated.h"

/**
 *
 */
UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced, CollapseCategories, DisplayName = "FunctionCall")
class CRABTOOLSUE5_API UFunctionTransitionCondition : public UTransitionCondition
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "StateMachine|Transition",
		meta = (AllowPrivateAccess, GetOptions = "GetFunctionOptions"))
	FName FunctionName;

	FTransitionDelegate Callback;

public:

	virtual bool Check() const override;

protected:

	void Initialize_Inner_Implementation() override;

private:
#if WITH_EDITOR
	UFUNCTION()
	TArray<FString> GetFunctionOptions() const;
#endif
};

/**
 *
 */
UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced, CollapseCategories, DisplayName = "FunctionCall")
class CRABTOOLSUE5_API UFunctionTransitionDataCondition : public UTransitionDataCondition
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "StateMachine|Transition",
		meta = (AllowPrivateAccess, GetOptions = "GetFunctionOptions"))
	FName FunctionName;

	FTransitionDataDelegate Callback;

public:

	virtual bool Check(UObject* Data) const override;

protected:

	void Initialize_Inner_Implementation() override;

private:
	#if WITH_EDITOR
		UFUNCTION()
		TArray<FString> GetFunctionOptions() const;
	#endif
};