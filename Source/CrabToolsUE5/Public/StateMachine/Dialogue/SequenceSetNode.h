#pragma once

#include "StateMachine/StateMachine.h"
#include "LevelSequencePlayer.h"
#include "SequenceSetNode.generated.h"

/* This node will set the sequence to be used to the sequence actor. */
UCLASS(Blueprintable, CollapseCategories, Category = "StateMachine|Dialogue")
class CRABTOOLSUE5_API USequenceSetNode : public UStateNode
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, Category="Sequence",
		meta=(AllowPrivateAccess))
	TSoftObjectPtr<ULevelSequence> Sequence;

	UPROPERTY(EditAnywhere, Category = "Sequence",
		meta = (AllowPrivateAccess))
	bool bAsyncLoad = false;
	
	UPROPERTY(EditAnywhere, Category = "Sequence",
		meta = (AllowPrivateAccess))
	bool bPlayOnSet = false;

public:

	USequenceSetNode();

protected:

	virtual void Initialize_Inner_Implementation() override;
	virtual void PostTransition_Inner_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, Category="Sequence|Loading")
	void OnLoadStart();
	virtual void OnLoadStart_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, Category = "Sequence|Loading")
	void OnLoadEnd();
	virtual void OnLoadEnd_Implementation() {}

private:
	UFUNCTION()
	void OnLoadNative();
};
