#include "StateMachine/StateMachine.h"
#include "MonologueNode.generated.h"

UCLASS(Blueprintable, Category = "StateMachine|Dialogue")
class CRABTOOLSUE5_API UMonologueNode : public UStateNode
{
	GENERATED_BODY()

	/* The map of choices text to the next state. */
	UPROPERTY(VisibleAnywhere, Category = "DialogueNode", meta = (ShowInnerProperties))
	TObjectPtr<class UMonologueData> Data;

	UPROPERTY()
	TObjectPtr<class UDialogueStateComponent> DialogueComponent;

public:

	UMonologueNode();

protected:

	#if WITH_EDITOR
		virtual void GetEmittedEvents(TSet<FName>& Events) const override;
	#endif

	virtual void Initialize_Inner_Implementation() override;
	virtual void Exit_Inner_Implementation() override;
	virtual void PostTransition_Inner_Implementation() override;

private:

	UFUNCTION()
	void HandleFinish(UMonologueData* MonologueData);
};