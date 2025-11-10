#include "Sequencer/SequencerStruct.h"

void USequencerBlueprintHelpers::ApplyAction(ULevelSequencePlayer* Player, const FSequencerAction& Action)
{
	if (!Player) { return; }

	for (const auto Act : Action.Action)
	{
		switch (Act)
		{
			case ESequenceActionType::PLAY: Player->Play(); break;
			case ESequenceActionType::PAUSE: Player->Pause(); break;
			case ESequenceActionType::PLAY_FROM_START:
				Player->Stop();
				Player->Play();
				break;
			case ESequenceActionType::STOP: Player->Stop(); break;
			case ESequenceActionType::TIME_STEP:
				Player->SetPlaybackPosition(Action.TimeStep);
		}
	}
}