#include "Actors/StateMachineActors/SMLevelSequenceActor.h"

ASMLevelSequenceActor::ASMLevelSequenceActor(const FObjectInitializer& ObjectInit): ALevelSequenceActor(ObjectInit)
{
	this->StateMachineComponent = CreateDefaultSubobject<UStateMachineComponent>(TEXT("StateMachineComponent"));
}

void ASMLevelSequenceActor::BeginPlay()
{
	if (this->CutsceneClass)
	{
		this->StateMachineComponent->CreateMachine(this->CutsceneClass);
	}

	Super::BeginPlay();
}

