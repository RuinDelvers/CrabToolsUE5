#include "Actors/Sequencer/SplineLevelSequenceActor.h"
#include "Components/SplineComponent.h"
#include "LevelSequencePlayer.h"

ASplineLevelSequenceActor::ASplineLevelSequenceActor(const FObjectInitializer& ObjectInit): Super(ObjectInit)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	this->Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	this->Spline->SetupAttachment(this->RootComponent);
}

void ASplineLevelSequenceActor::SetReferenceActor(AActor* NewReference)
{
	this->ReferenceActor = NewReference;
}

void ASplineLevelSequenceActor::BeginPlay()
{
	Super::BeginPlay();

	this->GetSequencePlayer()->Pause();
}

void ASplineLevelSequenceActor::Tick(float DeltaTime)
{
	if (this->ReferenceActor.IsValid())
	{
		FMovieSceneSequencePlaybackParams Step;

		Step.PositionType = EMovieScenePositionType::Time;
		Step.Time = this->GetSequencePlayer()->GetDuration().AsSeconds() * this->Spline->FindInputKeyClosestToWorldLocation(this->ReferenceActor->GetActorLocation());
		Step.UpdateMethod = EUpdatePositionMethod::Jump;


		this->GetSequencePlayer()->SetPlaybackPosition(Step);
	}

	Super::Tick(DeltaTime);
}
