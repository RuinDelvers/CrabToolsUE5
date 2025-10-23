#include "Actors/Triggers/EventTriggerBox.h"
#include "StateMachine/EventListener.h"
#include "Components/ShapeComponent.h"

AEventTriggerBox::AEventTriggerBox()
{
	// These are not meant to be physical blockers, just overlap detectors.
	this->GetCollisionComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEventTriggerBox::BeginPlay()
{
	Super::BeginPlay();

	this->GetCollisionComponent()->OnComponentBeginOverlap.AddDynamic(this, &AEventTriggerBox::OnBeginOverlap);
	this->GetCollisionComponent()->OnComponentEndOverlap.AddDynamic(this, &AEventTriggerBox::OnEndOverlap);
}

void AEventTriggerBox::OnBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (this->bUseData)
	{
		IEventListenerInterface::Execute_Event(OtherActor, this->BeginEvent);
	}
	else
	{
		IEventListenerInterface::Execute_EventWithData(OtherActor, this->BeginEvent, this->Data.LoadSynchronous());
	}	
}

void AEventTriggerBox::OnEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (this->bUseData)
	{
		IEventListenerInterface::Execute_Event(OtherActor, this->EndEvent);
	}
	else
	{
		IEventListenerInterface::Execute_EventWithData(OtherActor, this->EndEvent, this->Data.LoadSynchronous());
	}
}

#if WITH_EDITOR
TArray<FString> AEventTriggerBox::GetEventOptions() const
{
	TArray<FString> Names;

	for (auto& Object : this->Interfaces)
	{
		if (auto Interface = Object.LoadSynchronous())
		{
			for (auto& EventName : Interface->GetEvents())
			{
				Names.Add(EventName.ToString());
			}
		}
	}

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}

void AEventTriggerBox::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(AEventTriggerBox, bUseData))
	{
		if (!this->bUseData)
		{
			Data = nullptr;
		}
	}
}
#endif // WITH_EDITOR