#include "Ability/Movement/RotateToAbility.h"
#include "Utils/UtilsLibrary.h"

void URotateToAbility::SetTargetActor(AActor* Target)
{
	if (IsValid(Target))
	{
		this->TargetPoint = Target->GetActorLocation();
	}
	else
	{
		this->TargetPoint = this->GetOwner()->GetActorLocation();
	}
}

void URotateToAbility::SetTargetPoint(const FVector& Target)
{
	this->TargetPoint = Target;
}

void URotateToAbility::Tick_Inner_Implementation(float DeltaTime)
{
	FVector ToTarget = this->TargetPoint - this->GetOwner()->GetActorLocation();

	bool Complete = false;

	auto DeltaYaw = UUtilsLibrary::RotateAngleToDelta(
		this->GetOwner()->GetActorRotation().Yaw,
		ToTarget.Rotation().Yaw,
		this->RotateSpeed*DeltaTime,
		Complete);

	FQuat Rot(FRotator(0, DeltaYaw, 0));

	this->GetOwner()->AddActorLocalRotation(Rot);

	if (Complete)
	{
		this->Finish();
	}
}