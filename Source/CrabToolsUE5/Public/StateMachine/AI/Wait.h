#pragma once

#include "CoreMinimal.h"
#include "StateMachine/AI/BaseNode.h"
#include "Wait.generated.h"


UCLASS(Blueprintable, Category = "StateMachine|AI")
class CRABTOOLSUE5_API UAIWaitNode : public UAIBaseNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "StateMachine|AI",
		meta = (AllowPrivateAccess = true))
	float WaitTime = 1;

	FTimerHandle Callback;

public:

	UAIWaitNode();

	virtual void Enter_Inner_Implementation() override;
	virtual void Exit_Inner_Implementation() override;

	void OnWaitEnd();
};
