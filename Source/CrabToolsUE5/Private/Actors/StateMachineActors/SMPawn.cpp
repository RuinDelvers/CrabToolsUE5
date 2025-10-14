#include "Actors/StateMachineActors/SMPawn.h"

ASMPawn::ASMPawn(): APawn() {

	StateMachineComponent = CreateDefaultSubobject<UStateMachineComponent>(TEXT("StateMachineComponent"));
}



