#include "StateMachine/StateMachine.h"
#include "StateMachine/Dialogue/AbstractDialogueNode.h"
#include "DialogueNullNode.generated.h"

UCLASS(Blueprintable, CollapseCategories, Category = "StateMachine|Dialogue")
class CRABTOOLSUE5_API UDialogueNullNode : public UAbstractDialogueNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Flags")
	bool bUseOnEnter = true;

	UPROPERTY(EditAnywhere, Category = "Flags")
	bool bUseOnExit = true;

protected:

	virtual void Enter_Inner_Implementation() override;
	virtual void Exit_Inner_Implementation() override;
};