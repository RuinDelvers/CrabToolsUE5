#pragma once

#include "StateMachine/StateMachine.h"
#include "StateMachine/Dialogue/ICutsceneStateMachine.h"
#include "StateMachine/StateMachine.h"
#include "SequenceStateMachine.generated.h"

UCLASS(Blueprintable)
class USequenceStateMachine : public UStateMachine, public ICutsceneStateMachine
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Sequence")
	TSoftObjectPtr<ULevelSequence> Sequence;

	UPROPERTY(EditAnywhere, Category = "Sequence")
	ESequenceStatePlayPhase PlayPhase = ESequenceStatePlayPhase::UNSPECIFIED;

public:

	virtual TSoftObjectPtr<ULevelSequence> GetMachineSequence_Implementation() const override;
	virtual ESequenceStatePlayPhase GetPlayPhase_Implementation() const override;
};