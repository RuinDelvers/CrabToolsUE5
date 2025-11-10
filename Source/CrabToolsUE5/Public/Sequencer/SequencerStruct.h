#pragma once

#include "CoreMinimal.h"
#include "LevelSequencePlayer.h"
#include "SequencerStruct.generated.h"

UENUM()
enum class ESequenceActionType
{
	NONE UMETA(DisplayName = "None"),
	STOP UMETA(DisplayName = "Stop"),
	PLAY UMETA(DisplayName = "Play"),
	PAUSE UMETA(DisplayName = "Pause"),
	PLAY_FROM_START UMETA(DisplayName = "Play from Start"),
	TIME_STEP UMETA(DisplayName = "Time Step"),
};

USTRUCT(BlueprintType)
struct FSequencerAction
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sequencer")
	TArray<ESequenceActionType> Action;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sequencer")
	FMovieSceneSequencePlaybackParams TimeStep;
};

UCLASS()
class USequencerBlueprintHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="Sequencer")
	static void ApplyAction(ULevelSequencePlayer* Player, const FSequencerAction& Action);

};