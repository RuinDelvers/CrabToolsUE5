#include "Components/LockOnCamera/LockOnCamera.h"
#include "Components/LockOnCamera/LockOnInterface.h"
#include "GameFramework/Character.h"
#include "Utils/UtilsLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

ULockOnCamera::ULockOnCamera(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
	this->bForcePawnRotation = true;
	this->TargetArmLength = 400.0f;
	this->bUsePawnControlRotation = true; 

	// Create a follow camera
	this->FollowCamera = this->CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	this->FollowCamera->SetupAttachment(this, USpringArmComponent::SocketName); 
	this->FollowCamera->bUsePawnControlRotation = false;

	
}

void ULockOnCamera::BeginPlay() {
	Super::BeginPlay();
	APawn* POwner = Cast<APawn>(this->GetOwner());

	if (POwner) {
		this->PawnOwner = POwner;
	}

	if (ACharacter* c = Cast<ACharacter>(this->PawnOwner)) {
		this->CharacterOwner = c;
		this->bIsMovementOrienting = c->GetCharacterMovement()->bOrientRotationToMovement;
	}
	else {
		this->bIsMovementOrienting = false;
	}
}

void ULockOnCamera::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	

	if (this->IsLocked() && this->PawnOwner.IsValid()) {
		auto CharRot = this->GetDesiredRotation();
		this->PawnOwner->GetController()->SetControlRotation(CharRot);

		if (this->bForcePawnRotation) {
			if (this->bIsMovementOrienting) {
				FRotator Base = this->CharacterOwner->GetActorRotation();
				FRotator Delta = this->CharacterOwner->GetCharacterMovement()->RotationRate * DeltaTime;
				
				FRotator Goal(
					Delta.Pitch > 0 ? FMath::Clamp(UUtilsLibrary::RotateAngleTo(Base.Pitch, CharRot.Pitch, Delta.Pitch), -this->PitchAngle, this->PitchAngle) : Base.Pitch,
					Delta.Yaw > 0 ? UUtilsLibrary::RotateAngleTo(Base.Yaw, CharRot.Yaw, Delta.Yaw) : Base.Yaw,
					Delta.Roll > 0 ? UUtilsLibrary::RotateAngleTo(Base.Roll, CharRot.Roll, Delta.Roll) : Base.Roll);
				

				this->CharacterOwner->SetActorRotation(Goal);				
			}
			else {
				this->GetOwner()->SetActorRotation(CharRot);
			}
		}		
	}

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void ULockOnCamera::DetectCallback(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult) {

	this->AddTarget(OtherComp);
}

void ULockOnCamera::EndDetectCallback(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex) {

	this->RemoveTarget(OtherComp);
}

FRotator ULockOnCamera::GetDesiredRotation() const {
	if (this->CurrentTarget.IsValid()) {
		return FRotationMatrix::MakeFromX(this->CurrentTarget->GetComponentLocation() - this->GetComponentLocation()).Rotator();
	}
	else {
		return Super::GetDesiredRotation();
	}
	
}

void ULockOnCamera::LockOn() {
	if (this->DetectedComponents.Num() > 0) {
		this->bUsePawnControlRotation = false;
		this->CurrentTarget = this->DetectedComponents[0];

		if (this->bIsMovementOrienting) {
			this->CharacterOwner->GetCharacterMovement()->bOrientRotationToMovement = !this->bForcePawnRotation;
		}
	}
}

void ULockOnCamera::LockOff() {
	this->bUsePawnControlRotation = true;
	this->CurrentTarget = nullptr;

	if (this->bIsMovementOrienting) {
		this->CharacterOwner->GetCharacterMovement()->bOrientRotationToMovement = true;
	}
}

void ULockOnCamera::Toggle() {
	if (this->IsLocked()) {
		this->LockOff();
	}
	else {
		this->LockOn();
	}
}

void ULockOnCamera::AddTarget(USceneComponent* Target) {
	this->DetectedComponents.Add(Target);
}

void ULockOnCamera::RemoveTarget(USceneComponent* Target) {
	this->DetectedComponents.Remove(Target);

	if (this->CurrentTarget == Target) {
		this->LockOff();
	}
}

bool ULockOnCamera::IsLocked() {
	return this->CurrentTarget.IsValid();
}

AActor* ULockOnCamera::GetLockedActor() {
	if (this->CurrentTarget.IsValid()) {
		return this->CurrentTarget->GetOwner();
	}
	else {
		return nullptr;
	}
}

USceneComponent* ULockOnCamera::GetLockedComponent() {
	return this->CurrentTarget.Get();
}

FRotator ULockOnCamera::GetLookRotation() {
	if (this->CurrentTarget.IsValid()) {
		return FRotationMatrix::MakeFromX(this->CurrentTarget->GetComponentLocation() - this->GetComponentLocation()).Rotator();
	}
	else {
		return FRotator::ZeroRotator;
	}
	
}

void ULockOnCamera::SetForceRotation(bool Force) {
	this->bForcePawnRotation = Force;

	if (this->IsLocked() && this->bIsMovementOrienting) {
		this->CharacterOwner->GetCharacterMovement()->bOrientRotationToMovement = !Force;
	}
}

void ULockOnCamera::LerpControlRotation() {
	const float Add = 1.0f / 60;
	if (this->IsLocked() && this->bForcePawnRotation) {
		if (this->CharacterOwner.IsValid()) {
			if (this->CharacterOwner->GetCharacterMovement()->bOrientRotationToMovement) {

			}
		}
		else {
			auto CurRot = this->PawnOwner->GetControlRotation();
			auto CurDir = CurRot.RotateVector(FVector::ForwardVector);
			auto TarRot = this->GetDesiredRotation();
			auto TarDir = TarRot.RotateVector(FVector::ForwardVector);

			float angle = TarDir.CosineAngle2D(CurDir);
			
			if (1 - angle < Add) {
				this->PawnOwner->bUseControllerRotationYaw = true;
			}
		}
	}
}