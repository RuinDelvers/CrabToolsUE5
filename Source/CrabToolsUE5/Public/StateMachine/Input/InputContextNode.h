#pragma once

#include "StateMachine/StateMachine.h"
#include "GameFramework/Pawn.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystemInterface.h"
#include "InputContextNode.generated.h"

USTRUCT()
struct FMappingData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> Mapping;

	UPROPERTY(EditAnywhere, Category = "Input")
	int Priority = 0;

	UPROPERTY(EditAnywhere, Category = "Input")
	FModifyContextOptions Options;
};

/**
 * Node used to add and remove contexts for the enhanced input system.
 */
UCLASS(Blueprintable, EditInlineNew, DontCollapseCategories, Category = "StateMachine|Input")
class CRABTOOLSUE5_API UInputContextNode : public UStateNode
{
	GENERATED_BODY()

private:

	UPROPERTY(EditDefaultsOnly, Category="Input", meta=(AllowPrivateAccess))
	bool bClearContextOnExit = true;

	UPROPERTY(EditDefaultsOnly, Category = "Input", meta = (AllowPrivateAccess))
	bool bClearContextOnEnter = true;

	UPROPERTY(EditDefaultsOnly, Category = "Input", meta = (AllowPrivateAccess))
	TArray<FMappingData> ContextsToAdd;

	/* Contexts to remove upon enter. Ignored when bClearContextOnExit is true. */
	UPROPERTY(EditDefaultsOnly, Category = "Input", meta = (AllowPrivateAccess))
	TSet<TObjectPtr<UInputMappingContext>> ContextsToRemove;

protected:

	UPROPERTY(BlueprintReadOnly, Category="Pawn")
	TObjectPtr<APawn> PawnOwner;

public:

	
protected:

	virtual void Initialize_Inner_Implementation() override;
	virtual void SetActive_Inner_Implementation(bool bNewActive) override;
};