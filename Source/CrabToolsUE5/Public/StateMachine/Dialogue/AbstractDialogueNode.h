#pragma once

#include "StateMachine/StateMachine.h"
#include "Components/Dialogue/DialogueComponent.h"
#include "AbstractDialogueNode.generated.h"



/**
 * Base Dialogue node. Holds a reference to the dialogue component.
 */
UCLASS(Abstract, Blueprintable, Category = "StateMachine|Dialogue")
class CRABTOOLSUE5_API UAbstractDialogueNode : public UStateNode
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Dialogue",
		meta=(AllowPrivateAccess))
	TObjectPtr<UDialogueStateComponent> DialogueComponent;

public:

	FORCEINLINE UDialogueStateComponent* GetDialogueComponent() const { return this->DialogueComponent; }

protected:
		
	virtual void Initialize_Inner_Implementation() override;

};