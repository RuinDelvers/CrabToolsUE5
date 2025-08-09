#pragma once

#include "StateMachine/StateMachine.h"
#include "IsTypeTransition.generated.h"



/**
 *
 */
UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced, DisplayName = "True")
class CRABTOOLSUE5_API UIsTypeTransitionDataCondition : public UTransitionDataCondition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Transition")
	TSubclassOf<UObject> Class;

	UPROPERTY(EditAnywhere, Category = "Transition")
	TSet<TSubclassOf<UInterface>> Interface;

public:

	virtual bool Check(UObject* Obj) const override;
};

