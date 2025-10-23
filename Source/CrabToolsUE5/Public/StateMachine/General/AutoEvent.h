#pragma once

#include "CoreMinimal.h"
#include "StateMachine/StateMachine.h"
#include "AutoEvent.generated.h"

/**
 * Node that automatically sends an event at PostTransition. Similar to Conduit Nodes for
 * AnimGraphs.
 */
UCLASS(Blueprintable, Category = "StateMachine")
class CRABTOOLSUE5_API UAutoEventNode : public UStateNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "StateMachine", meta = (AllowPrivateAccess))
	FName EmittedEvent;

public:

	virtual void Enter_Inner_Implementation() override;

	#if WITH_EDITORONLY_DATA
		virtual void GetEmittedEvents(TSet<FName>& Events) const override;
	#endif
};
