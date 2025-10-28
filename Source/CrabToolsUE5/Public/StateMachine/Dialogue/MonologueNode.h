#include "StateMachine/Dialogue/AbstractDialogueNode.h"
#include "MonologueNode.generated.h"

UCLASS(Blueprintable, Category = "StateMachine|Dialogue")
class CRABTOOLSUE5_API UMonologueNode : public UAbstractDialogueNode
{
	GENERATED_BODY()

	/* The map of choices text to the next state. */
	UPROPERTY(VisibleAnywhere, Category = "DialogueNode", meta = (ShowInnerProperties))
	TObjectPtr<class UMonologueData> Data;

	UPROPERTY(EditAnywhere, Category = "DialogueNode", meta = (AllowPrivateAccess))
	bool bNullOnExit = false;

public:

	UMonologueNode();

protected:

	#if WITH_EDITOR
		virtual void GetEmittedEvents(TSet<FName>& Events) const override;
	#endif

	virtual void Initialize_Inner_Implementation() override;
	virtual void Exit_Inner_Implementation() override;
	virtual void Enter_Inner_Implementation() override;

private:

	UFUNCTION()
	void HandleFinish(UMonologueData* MonologueData);
};