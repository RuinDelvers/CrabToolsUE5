#include "StateMachine/Physics/OnHitNode.h"
#include "Components/PrimitiveComponent.h"

UOnHitNode::UOnHitNode(): Component(UPrimitiveComponent::StaticClass())
{

}

void UOnHitNode::Enter_Inner_Implementation()
{
	if (auto Comp = Cast<UPrimitiveComponent>(this->GetActorOwner()->FindComponentByClass(this->Component.Get())))
	{
		Comp->OnComponentHit.AddDynamic(this, &UOnHitNode::OnHitDetected_Internal);
	}
}

void UOnHitNode::Exit_Inner_Implementation()
{
	if (auto Comp = Cast<UPrimitiveComponent>(this->GetActorOwner()->FindComponentByClass(this->Component.Get())))
	{
		Comp->OnComponentHit.RemoveAll(this);
	}
}

void UOnHitNode::OnHitDetected_Internal(
	UPrimitiveComponent* HitComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit)
{
	if (this->Active())
	{
		this->OnHitDetected(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
	}
}