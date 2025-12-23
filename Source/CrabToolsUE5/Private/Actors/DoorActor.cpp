#include "Actors/DoorActor.h"
#include "Curves/CurveVector.h"
#include "UObject/ConstructorHelpers.h"

#pragma region Door Component

UDoorActorMeshComponent::UDoorActorMeshComponent()
{
	static ConstructorHelpers::FObjectFinder<UCurveVector> RotationFinder(TEXT("/CrabToolsUE5/Data/Curves/DefaultDoorRotationCurve_C.DefaultDoorRotationCurve_C"));

	if (RotationFinder.Succeeded())
	{
		this->Rotation = RotationFinder.Object;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find default rotation curve for door mesh."));
	}

	static ConstructorHelpers::FObjectFinder<UCurveVector> TranslationFinder(TEXT("/CrabToolsUE5/Data/Curves/DefaultDoorTranslationCurve_C.DefaultDoorTranslationCurve_C"));

	if (TranslationFinder.Succeeded())
	{
		this->Translation = TranslationFinder.Object;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find default translation curve for door mesh."));
	}
}

void UDoorActorMeshComponent::BeginPlay()
{
	Super::BeginPlay();

	this->InitPosition = this->GetComponentLocation();
	this->InitRotation = this->GetComponentRotation();
}

void UDoorActorMeshComponent::Update(float Alpha)
{
	auto NewRotationVector = this->Rotation->GetVectorValue(Alpha);
	auto NewRotation = FRotator(NewRotationVector.X, NewRotationVector.Z, NewRotationVector.Y);
	auto NewTranslation = this->Translation->GetVectorValue(Alpha);

	this->SetRelativeRotation(NewRotation);
}

#pragma endregion

#pragma region DoorActor

ADoorActor::ADoorActor()
: PlayRate(1),
	CurrentAlpha(0)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	this->MovementTimeline = CreateDefaultSubobject<UTimelineComponent>(
		TEXT("DoorRotationTimeline"));

	this->RootComponent->SetMobility(EComponentMobility::Movable);
	this->SetActorHiddenInGame(false);
	this->SetActorEnableCollision(true);
	

	static ConstructorHelpers::FObjectFinder<UCurveFloat> MovementFinder(TEXT("/CrabToolsUE5/Data/Curves/DefaultDoorMovementCurve_C.DefaultDoorMovementCurve_C"));

	if (MovementFinder.Succeeded())
	{
		this->MovementCurve = MovementFinder.Object;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find default movement curve for door mesh."));
	}
}

// Called when the game starts or when spawned
void ADoorActor::BeginPlay()
{
	Super::BeginPlay();

	this->Axis = this->GetActorRotation().RotateVector(FVector::UpVector);
	this->BaseRotation = this->GetActorRotation().Quaternion();

	this->MovementTimeline->SetPlayRate(this->PlayRate);
	this->MovementTimeline->SetLooping(false);
	this->MovementTimeline->SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame);

	this->MovementTrack.BindDynamic(this, &ADoorActor::UpdatePosition);
	this->EndMovementTrack.BindDynamic(this, &ADoorActor::FinishMovement);

	if (this->MovementCurve != nullptr) {
		this->MovementTimeline->AddInterpFloat(this->MovementCurve, MovementTrack);
		this->MovementTimeline->SetTimelineFinishedFunc(EndMovementTrack);
	}
}

void ADoorActor::ActivateDoor(bool OpenQ)
{
	this->MovementTimeline->Stop();
	this->MovementTimeline->SetNewTime(this->CurrentAlpha);

	if (OpenQ)
	{
		this->OnDoorStateChanged(EDoorState::OPENING);
		this->MovementTimeline->Play();
	}
	else
	{
		this->MovementTimeline->Reverse();
	}
}

void ADoorActor::OnDoorStateChanged(EDoorState NewState)
{
	this->State = NewState;
	this->OnDoorStateChanged_Inner();
}

void ADoorActor::UpdatePosition(float alpha) 
{
	this->CurrentAlpha = alpha;

	for (const auto& Doors : this->GetDoorComponents())
	{
		Doors->Update(alpha);
	}
}

void ADoorActor::FinishMovement()
{
	this->OnFinishMovementEvent.Broadcast();
}

void ADoorActor::SetPlayRate(float NewPlayRate)
{
	this->PlayRate = NewPlayRate;
	this->MovementTimeline->SetPlayRate(this->PlayRate);
}

void ADoorActor::ToggleDoorEditor()
{
	switch (this->State)
	{
		case EDoorState::OPEN: 
			this->OnDoorStateChanged(EDoorState::CLOSED);
			this->MovementTimeline->SetNewTime(this->MovementTimeline->GetTimelineLength());

			for (const auto& Doors : this->GetDoorComponents())
			{
				Doors->Update(0.0);
			}
			
			break;
		case EDoorState::OPENING:
			this->OnDoorStateChanged(EDoorState::CLOSED);
			this->MovementTimeline->SetNewTime(this->MovementTimeline->GetTimelineLength());

			for (const auto& Doors : this->GetDoorComponents())
			{
				Doors->Update(0.0);
			}
			break;
		case EDoorState::CLOSED:
			this->OnDoorStateChanged(EDoorState::OPEN);
			this->MovementTimeline->SetNewTime(0);

			for (const auto& Doors : this->GetDoorComponents())
			{
				Doors->Update(1.0);
			}
			break;
		case EDoorState::CLOSING:
			this->OnDoorStateChanged(EDoorState::OPEN);
			this->MovementTimeline->SetNewTime(0);

			for (const auto& Doors : this->GetDoorComponents())
			{
				Doors->Update(1.0);
			}
			break;
	}
	this->MarkPackageDirty();
}

void ADoorActor::ToggleDoor()
{
	switch (this->State)
	{
		case EDoorState::OPEN: this->ActivateDoor(false); break; 
		case EDoorState::OPENING: this->ActivateDoor(false); break;
		case EDoorState::CLOSED: this->ActivateDoor(true); break;
		case EDoorState::CLOSING: this->ActivateDoor(true); break; 
	}
}

TArray<TObjectPtr<UDoorActorMeshComponent>>& ADoorActor::GetDoorComponents()
{
	if (this->Components.Num() == 0)
	{
		for (const auto& Comps : this->GetComponents())
		{
			if (auto DoorComp = Cast<UDoorActorMeshComponent>(Comps))
			{
				this->Components.Add(DoorComp);
			}
		}
	}

	return this->Components;
}

#pragma endregion