#pragma once

#include "StateMachine/StateMachine.h"
#include "LevelSequencePlayer.h"
#include "SequenceNullProgressNode.generated.h"

/* 
 * This node will check to see if the owning sequence actor has a 
 * null sequence. If so, it'll emit an event. This is useful when
 * utilizing the standard SM pattern for dialogue, but did not want
 * to specify a cutscene.
 */
UCLASS(Blueprintable, CollapseCategories, Category = "StateMachine|Dialogue")
class CRABTOOLSUE5_API USequenceNullProgressNode : public UStateNode
{
	GENERATED_BODY()


public:

	USequenceNullProgressNode();

protected:

	virtual void PostTransition_Inner_Implementation() override;
};
