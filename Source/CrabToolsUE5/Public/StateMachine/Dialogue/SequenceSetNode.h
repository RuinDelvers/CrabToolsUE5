#pragma once

#include "StateMachine/StateMachine.h"
#include "StateMachine/Dialogue/ICutsceneStateMachine.h"
#include "SequenceSetNode.generated.h"

class UGenericPropertyBinding;

UENUM()
enum class ESequenceSource
{
	INLINE UMETA(DisplayName="Inline"),
	PROPERTY UMETA(DisplayName = "Property"),
	INTERFACE UMETA(DisplayName = "Interface"),
	PARENT_INTERFACE UMETA(DisplayName = "Parent Interface"),
};

/* This node will set the sequence to be used to the sequence actor. */
UCLASS(Blueprintable, CollapseCategories, Category = "StateMachine|Dialogue")
class CRABTOOLSUE5_API USequenceSetNode : public UStateNode
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, Category = "Sequence",
		meta = (AllowPrivateAccess))
	ESequenceSource SequenceSource = ESequenceSource::INLINE;

	UPROPERTY(VisibleAnywhere, Category = "Sequence",
		meta = (AllowPrivateAccess, ShowInnerProperties,
			EditCondition="SequenceSource==ESequenceSource::PROPERTY",
			EditConditionHides))
	TObjectPtr<UGenericPropertyBinding> SeqBinding;

	UPROPERTY(EditAnywhere, Category = "Sequence",
		meta = (AllowPrivateAccess, ShowInnerProperties,
			EditCondition="SequenceSource==ESequenceSource::INLINE",
			EditConditionHides))
	TSoftObjectPtr<ULevelSequence> Sequence;

	UPROPERTY(EditAnywhere, Category = "Sequence",
		meta = (AllowPrivateAccess))
	bool bAsyncLoad = false;
	
	UPROPERTY(EditAnywhere, Category = "Sequence",
		meta = (AllowPrivateAccess))
	ESequenceStatePlayPhase PlayPhase = ESequenceStatePlayPhase::ON_LOAD;

	UPROPERTY(EditAnywhere, Category = "Sequence",
		meta = (AllowPrivateAccess,
			EditCondition="SequenceSource==ESequenceSource::INTERFACE",
			EditConditionHides))
	bool bAllowParentInterface = true;

public:

	USequenceSetNode();

protected:

	virtual void Initialize_Inner_Implementation() override;
	virtual void Enter_Inner_Implementation() override;
	virtual void Exit_Inner_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, Category="Sequence|Loading")
	void OnLoadStart();
	virtual void OnLoadStart_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, Category = "Sequence|Loading")
	void OnLoadEnd();
	virtual void OnLoadEnd_Implementation() {}

private:
	UFUNCTION()
	void OnLoadNative(TSoftObjectPtr<ULevelSequence> Ptr);

	void LoadInline();
	void LoadProperty();
	void LoadInterface();

	void LoadSoftPtr(TSoftObjectPtr<ULevelSequence> Ptr);

	ESequenceStatePlayPhase GetPlayPhase() const;
};
