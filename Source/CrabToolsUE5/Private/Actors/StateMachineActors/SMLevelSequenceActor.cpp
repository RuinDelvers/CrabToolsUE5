#include "Actors/StateMachineActors/SMLevelSequenceActor.h"

ASMLevelSequenceActor::ASMLevelSequenceActor(const FObjectInitializer& ObjectInit): ALevelSequenceActor(ObjectInit)
{
	StateMachineComponent = CreateDefaultSubobject<UStateMachineComponent>(TEXT("StateMachineComponent"));
}



