#pragma once

#include "ICutsceneStateMachine.generated.h"

UENUM(BlueprintType)
enum class ESequenceStatePlayPhase: uint8
{
    UNSPECIFIED UMETA(DisplayName = "Unspecified"),
    ON_LOAD     UMETA(DisplayName = "On Load"),
    ON_EXIT     UMETA(DisplayName = "On Exit"),
};

class ULevelSequence;

/* Interface for statemachines which are used for cutscenes..*/
UINTERFACE(MinimalAPI)
class UCutsceneStateMachine : public UInterface
{
    GENERATED_BODY()
};

class ICutsceneStateMachine
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Sequence")
    TSoftObjectPtr<ULevelSequence> GetMachineSequence() const;
    virtual TSoftObjectPtr<ULevelSequence> GetMachineSequence_Implementation() const { return nullptr; }

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Sequence")
    ESequenceStatePlayPhase GetPlayPhase() const;
    virtual ESequenceStatePlayPhase GetPlayPhase_Implementation() const { return ESequenceStatePlayPhase::UNSPECIFIED; }
};