#pragma once

#include "StateMachine/StateMachine.h"
#include "LevelSequencePlayer.h"
#include "SequenceActionNode.generated.h"

UENUM()
enum class ESequenceActionType
{
	NONE UMETA(DisplayName="None"),
	STOP UMETA(DisplayName = "Stop"),
	PLAY UMETA(DisplayName = "Play"),
	PAUSE UMETA(DisplayName = "Pause"),
	PLAY_FROM_START UMETA(DisplayName = "Play from Start"),
};

UCLASS(Blueprintable, CollapseCategories, Category = "StateMachine|Dialogue")
class CRABTOOLSUE5_API USequenceActionNode : public UStateNode
{
	GENERATED_BODY()

private:

	UPROPERTY(Transient)
	TObjectPtr<ULevelSequencePlayer> Player;

	UPROPERTY(EditAnywhere, Category="Pausing")
	ESequenceActionType EnterAction = ESequenceActionType::NONE;

	UPROPERTY(EditAnywhere, Category = "Pausing")
	ESequenceActionType ExitAction = ESequenceActionType::NONE;


protected:

	virtual void Initialize_Inner_Implementation() override;
	virtual void Enter_Inner_Implementation() override;
	virtual void Exit_Inner_Implementation() override;

private:

	void ApplyAction(ESequenceActionType Action);
};
