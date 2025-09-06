#include "Components/UIDComponent.h"

void UUIDComponent::OnComponentCreated()
{
	Super::OnComponentCreated();

	if (this->GetWorld() && this->ComponentIsInPersistentLevel(false))
	{
		this->GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UUIDComponent::Callback);
	}
}

void UUIDComponent::Callback()
{
	if (!this->ActorUID.IsValid())
	{
		this->ActorUID = FGuid::NewGuid();
		this->Modify();
	}
}