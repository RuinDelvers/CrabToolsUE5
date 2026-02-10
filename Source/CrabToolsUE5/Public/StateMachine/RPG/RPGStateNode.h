#pragma once

#include "StateMachine/StateMachine.h"
#include "RPGStateNode.generated.h"

class URPGComponent;

/**
 * Abstract utility node meant to be extended. Holds a reference to this node's ActorOwner's rpg component.
 */
UCLASS(Abstract, Blueprintable, Category = "StateMachine")
class CRABTOOLSUE5_API URPGStateNode : public UStateNode
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadOnly, BlueprintReadOnly, Category = "RPG",
		meta = (AllowPrivateAccess))
	TObjectPtr<URPGComponent> Component;

	#if WITH_EDITORONLY_DATA
		UPROPERTY(EditAnywhere, Category="Debug")
		bool bErrorOnMissingComponent = true;
	#endif

protected:

	virtual void Initialize_Inner_Implementation() override;
};