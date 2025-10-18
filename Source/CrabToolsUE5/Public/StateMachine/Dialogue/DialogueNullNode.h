#include "StateMachine/StateMachine.h"
#include "DialogueNullNode.generated.h"

UCLASS(Blueprintable, CollapseCategories, Category = "StateMachine|Dialogue")
class CRABTOOLSUE5_API UDialogueNullNode : public UStateNode
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<class UDialogueStateComponent> DialogueComponent;

	UPROPERTY(EditAnywhere, Category="Flags")
	bool bUseOnEnter = true;

	UPROPERTY(EditAnywhere, Category = "Flags")
	bool bUseOnExit = true;

protected:

	virtual void Initialize_Inner_Implementation() override;
	virtual void Enter_Inner_Implementation() override;
	virtual void Exit_Inner_Implementation() override;
};