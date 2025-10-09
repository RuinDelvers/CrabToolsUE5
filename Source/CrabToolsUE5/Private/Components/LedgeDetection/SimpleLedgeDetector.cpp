#include "Components/LedgeDetection/SimpleLedgeDetector.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

USimpleLedgeDetectorComponent::USimpleLedgeDetectorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void USimpleLedgeDetectorComponent::BeginPlay()
{
	Super::BeginPlay();

	this->Collision = this->GetOwner()->GetComponentByClass<UCapsuleComponent>();
	this->Movement = this->GetOwner()->GetComponentByClass<UCharacterMovementComponent>();

	if (this->bUseCapsuleRadiusForForward)
	{
		auto Component = this->GetOwner()->GetComponentByClass<UCapsuleComponent>();
		this->ForwardDistance = Component->GetScaledCapsuleRadius() + ForwardDistance;
	}

	this->TraceParams.TraceTag = FName(FString::Printf(TEXT("LedgeTrace - %s"), *this->GetOwner()->GetName()));
	this->TraceParams.bTraceComplex = this->bTraceComplex;
	this->TraceParams.AddIgnoredActor(this->GetOwner());
}

void USimpleLedgeDetectorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	this->PerformTrace();	
}

void USimpleLedgeDetectorComponent::PerformTrace()
{
	//FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), this->bTraceComplex, this->GetOwner());

	auto Height = Collision->GetScaledCapsuleHalfHeight();
	auto Radius = Collision->GetScaledCapsuleRadius();
	auto Location = this->Collision->GetComponentLocation();
	auto Yaw = this->GetOwner()->GetActorRotation().Yaw;
	FVector Forward = FVector::ForwardVector.RotateAngleAxis(Yaw, FVector::UpVector) * ForwardDistance;
	FVector Base = Location + Forward +  FVector(0, 0, AboveLedgeDistance + Height);
	FVector FixedTarget = Location + Forward - FVector(0, 0, BelowLedgeDistance + Height);

	bool FoundTarget = this->GetWorld()->LineTraceSingleByChannel(
		Result,
		Base,
		FixedTarget,
		this->TraceChannel,
		TraceParams);

	#if WITH_EDITORONLY_DATA
		FColor DebugColor = FColor::Black;
	#endif

	if (FoundTarget)
	{
		auto DistSq = (Result.ImpactPoint - Base).SquaredLength();

		if (DistSq < FMath::Pow(this->AboveLedgeDistance, 2))
		{
			#if WITH_EDITORONLY_DATA
				DebugColor = FColor::Blue;
				
			#endif
			this->AboveLedgeDetected();
		}
		else if (DistSq < FMath::Pow(this->AboveLedgeDistance + 2 * Height - Radius, 2))
		{
			#if WITH_EDITORONLY_DATA
				DebugColor = FColor::Green;
			#endif
			this->MiddleLedgeDetected();
		}
		else if (DistSq > FMath::Pow(this->AboveLedgeDistance + 2 * Height + 2*Radius, 2) && DistSq < FMath::Pow(this->AboveLedgeDistance + this->BelowLedgeDistance + 2 * Height + 2 * Radius, 2))
		{
			#if WITH_EDITORONLY_DATA
				DebugColor = FColor::Red;
			#endif
			this->BelowLedgeDetected();
		}
	}

	#if WITH_EDITORONLY_DATA
		if (this->bDrawDebug)
		{
			DrawDebugLine(this->GetWorld(), Base, FixedTarget, DebugColor, false, 0.25f, 0, 5.0f);
			if (FoundTarget)
			{
				DrawDebugSphere(this->GetWorld(), Result.ImpactPoint, 10, 12, DebugColor, false, 0.25f);
			}
		}
	#endif //WITH_EDITORONLY_DATA
}

void USimpleLedgeDetectorComponent::AddActivator(UObject* Obj)
{
	auto Amt = this->Activators.Num();
	this->Activators.Add(Obj);

	if (Amt == 0)
	{
		this->SetComponentTickEnabled(true);
	}
}

void USimpleLedgeDetectorComponent::RemoveActivator(UObject* Obj)
{
	this->Activators.Remove(Obj);

	if (this->Activators.Num() == 0)
	{
		this->SetComponentTickEnabled(false);
	}
}

void USimpleLedgeDetectorComponent::SetOwnerLocationAtResult() const
{
	if (this->Result.bBlockingHit)
	{
		auto Component = this->GetOwner()->GetComponentByClass<UCapsuleComponent>();
		this->GetOwner()->SetActorLocation(this->Result.ImpactPoint + FVector(0, 0, Component->GetScaledCapsuleHalfHeight()));
	}
}

void USimpleLedgeDetectorComponent::OnComponentCreated()
{
	Super::OnComponentCreated();
}

ULedgeDetectorActivationComponent::ULedgeDetectorActivationComponent()
{
	this->SetCollisionResponseToAllChannels(ECR_Ignore);
	this->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void ULedgeDetectorActivationComponent::BeginPlay() 
{
	Super::BeginPlay();

	this->SetCollisionObjectType(this->OverlapChannel);
	this->OnComponentBeginOverlap.AddDynamic(this, &ULedgeDetectorActivationComponent::OnBeginOverlap);
	this->OnComponentEndOverlap.AddDynamic(this, &ULedgeDetectorActivationComponent::OnEndOverlap);
}

void ULedgeDetectorActivationComponent::OnBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (auto LedgeDetect = OtherActor->GetComponentByClass<USimpleLedgeDetectorComponent>())
	{
		LedgeDetect->AddActivator(this);
	}
}

void ULedgeDetectorActivationComponent::OnEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (auto LedgeDetect = OtherActor->GetComponentByClass<USimpleLedgeDetectorComponent>())
	{
		LedgeDetect->RemoveActivator(this);
	}
}