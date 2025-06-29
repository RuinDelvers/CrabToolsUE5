#include "Actors/DoorActor.h"
#include "Curves/CurveVector.h"

#pragma region Door Component
TWeakObjectPtr<UCurveVector> UDoorActorMeshComponent::DefaultRotationCurve = nullptr;
TWeakObjectPtr<UCurveVector> UDoorActorMeshComponent::DefaultTranslationCurve = nullptr;

UCurveVector* UDoorActorMeshComponent::GetDefaultRotationCurve()
{
	if (!DefaultRotationCurve.IsValid())
	{
		DefaultRotationCurve = NewObject<UCurveVector>();

		DefaultRotationCurve->FloatCurves[1].UpdateOrAddKey(0, 0);
		DefaultRotationCurve->FloatCurves[1].UpdateOrAddKey(1, 90);

		DefaultRotationCurve->AddToRoot();
	}

	return DefaultRotationCurve.Get();
}

UCurveVector* UDoorActorMeshComponent::GetDefaultTranslationCurve()
{
	if (!DefaultTranslationCurve.IsValid())
	{
		DefaultTranslationCurve = NewObject<UCurveVector>();

		DefaultTranslationCurve->AddToRoot();
	}

	return DefaultTranslationCurve.Get();
}

UDoorActorMeshComponent::UDoorActorMeshComponent()
{
	this->Rotation = GetDefaultRotationCurve();
	this->Translation = GetDefaultTranslationCurve();
}

void UDoorActorMeshComponent::BeginPlay()
{
	Super::BeginPlay();

	this->InitPosition = this->GetComponentLocation();
	this->InitRotation = this->GetComponentRotation();
}

void UDoorActorMeshComponent::Update(float Alpha)
{

	auto NewRotation = FMath::Lerp(
		this->InitRotation,
		this->InitRotation + this->Rotation->GetVectorValue(Alpha).Rotation(),
		Alpha);

	auto NewTranslation = FMath::Lerp(
		this->InitPosition,
		this->InitPosition + this->Rotation->GetVectorValue(Alpha),
		Alpha);

	this->SetRelativeRotation(NewRotation);
}

#pragma endregion

#pragma region DoorActor

TWeakObjectPtr<UCurveFloat> ADoorActor::DefaultForwardCurve = nullptr;
TWeakObjectPtr<UCurveFloat> ADoorActor::DefaultReverseCurve = nullptr;

UCurveFloat* ADoorActor::GetDefaultForwardCurve()
{
	if (!ADoorActor::DefaultForwardCurve.IsValid())
	{
		ADoorActor::DefaultForwardCurve = NewObject<UCurveFloat>();

		DefaultForwardCurve->FloatCurve.UpdateOrAddKey(0.f, 0.f);
		DefaultForwardCurve->FloatCurve.UpdateOrAddKey(1.f, 1.f);

		ADoorActor::DefaultForwardCurve.Get()->AddToRoot();
	}

	return ADoorActor::DefaultForwardCurve.Get();
}

UCurveFloat* ADoorActor::GetDefaultReverseCurve()
{
	if (!ADoorActor::DefaultForwardCurve.IsValid())
	{
		ADoorActor::DefaultReverseCurve = NewObject<UCurveFloat>();

		DefaultReverseCurve->FloatCurve.UpdateOrAddKey(0.f, 0.f);
		DefaultReverseCurve->FloatCurve.UpdateOrAddKey(1.f, 1.f);

		ADoorActor::DefaultReverseCurve.Get()->AddToRoot();
	}

	return ADoorActor::DefaultReverseCurve.Get();
}

ADoorActor::ADoorActor()
: PlayRate(1),
	AngleDelta(90),
	CurrentAlpha(0)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	this->MovementTimeline = CreateDefaultSubobject<UTimelineComponent>(
		TEXT("DoorRotationTimeline"));

	this->RootComponent->SetMobility(EComponentMobility::Movable);
	this->SetActorHiddenInGame(false);
}

// Called when the game starts or when spawned
void ADoorActor::BeginPlay()
{
	Super::BeginPlay();

	for (const auto& Comps : this->GetComponents())
	{
		if (auto DoorComp = Cast<UDoorActorMeshComponent>(Comps))
		{
			this->Components.Add(DoorComp);
		}
	}

	this->Axis = this->GetActorRotation().RotateVector(FVector::UpVector);
	this->BaseRotation = this->GetActorRotation().Quaternion();

	if (!this->MovementCurve)
	{
		this->MovementCurve = this->GetDefaultForwardCurve();
	}	

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
		this->MovementTimeline->Play();
	}
	else
	{
		this->MovementTimeline->Reverse();
	}
}

void ADoorActor::UpdatePosition(float alpha) 
{
	this->CurrentAlpha = alpha;

	for (const auto& Doors : this->Components)
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

#pragma endregion