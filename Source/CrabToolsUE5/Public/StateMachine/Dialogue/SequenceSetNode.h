#pragma once

#include "Properties/GenericPropertyBinding.h"
#include "StateMachine/StateMachine.h"
#include "StateMachine/Dialogue/ICutsceneStateMachine.h"
#include "SequenceSetNode.generated.h"

UENUM()
enum class ESequenceSource
{
	INLINE UMETA(DisplayName="Inline"),
	PROPERTY UMETA(DisplayName = "Property"),
	INTERFACE UMETA(DisplayName = "Interface"),
	PARENT_INTERFACE UMETA(DisplayName = "Parent Interface"),
};

/*
 * This node will set the sequence to be used to the sequence actor. This uses a property binding to locate
 * a sequence asset. This is often used in conjunction of SequenceActionNode to also play the node after loading (i.e. on exit).
 */
UCLASS(Blueprintable, CollapseCategories, Category = "StateMachine|Dialogue")
class CRABTOOLSUE5_API USequenceSetNode : public UStateNode
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, Category = "Sequence",
		meta = (AllowPrivateAccess))
	FGenericObjectPropertyBinding Binding;

	UPROPERTY(EditAnywhere, Category = "Sequence",
		meta = (AllowPrivateAccess))
	bool bAsyncLoad = false;
	

public:

	USequenceSetNode();

protected:

	virtual void Enter_Inner_Implementation() override;

private:
};
