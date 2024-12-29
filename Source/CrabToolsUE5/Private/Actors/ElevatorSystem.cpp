// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/ElevatorSystem.h"

// Sets default values
AElevatorSystem::AElevatorSystem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	this->MovementTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("ElevatorMotionTimeline"));
	this->State.Sequence = CreateDefaultSubobject<UCyclicPathSequence>(TEXT("ElevatorSequence"));
}

// Called when the game starts or when spawned
void AElevatorSystem::BeginPlay()
{
	Super::BeginPlay();

	this->MovementTimeline->SetLooping(false);
	this->MovementTimeline->SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame);

	this->MovementTrack.BindDynamic(this, &AElevatorSystem::UpdatePosition);
	this->EndMovementTrack.BindDynamic(this, &AElevatorSystem::FinishedMovement);

	if (this->MovementCurve != nullptr) {
		this->MovementTimeline->AddInterpFloat(this->MovementCurve, MovementTrack);
		this->MovementTimeline->SetTimelineFinishedFunc(EndMovementTrack);
	}
}

void AElevatorSystem::Step()
{
	this->State.Step();
	this->StartMovement();
}

void AElevatorSystem::StartMovement()
{
	this->MovementTimeline->PlayFromStart();
}

void AElevatorSystem::UpdatePosition(float alpha) {
	
	alpha = FMath::Clamp(alpha, 0, 1);

	auto Start = this->GetMovementStart();
	this->SetActorLocation(Start + this->GetMovementDirection() * alpha);
}

void AElevatorSystem::FinishedMovement()
{
	if (this->State)
	{
		this->SetActorLocation(this->State.Point());
	}

	if (this->TargetIndex >= 0)
	{
		if (this->State.Target() == this->TargetIndex)
		{
			this->OnArrivedEvent.Broadcast(this->TargetIndex);
			this->TargetIndex = ElevatorSystem::Constants::NULL_TARGET;			
		}
		else
		{
			this->Step();
		}
	}
}

FVector AElevatorSystem::GetMovementStart() const
{
	if (this->State)
	{
		return this->State.Point();
	}
	else {
		return this->GetActorLocation();
	}
}

FVector AElevatorSystem::GetMovementDirection() const
{
	if (this->State)
	{
		auto P1 = this->GetMovementStart();
		auto P2 = this->State.Point();
		
		return P2 - P1;
	}
	else
	{
		return FVector::Zero();
	}
}

void AElevatorSystem::PostFinishCallback_Implementation() {}


void AElevatorSystem::GoToIndex(int i)
{
	if (auto Seq = Cast<UCyclicPathSequence>(this->State.Sequence))
	{
		Seq->SetDirection(i);
	}
	this->TargetIndex = i;
	this->Step();
}