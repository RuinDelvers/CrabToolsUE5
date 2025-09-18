#include "StateMachine/General/InteractionStateNode.h"
#include "StateMachine/Logging.h"

void UInteractionStateNode::Initialize_Inner_Implementation()
{
	if (auto Actor = this->GetActorOwner())
	{
		if (auto FoundComponent = Actor->FindComponentByClass<UInteractableComponent>())
		{
			this->Component = FoundComponent;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Warning: Actor %s did not have an interactable component."), *Actor->GetName());
		}
	}
	else
	{
		UE_LOG(LogStateMachine, Error, TEXT("Error: UInteractionStateNode not placed into actor's state machine."));
	}
}

void UInteractionStateNode::Enter_Inner_Implementation()
{
	for (const auto& Data : this->EnterStateMap)
	{
		Component->SetInteractionState(Data.Key, Data.Value);
	}
}
void UInteractionStateNode::Exit_Inner_Implementation()
{
	for (const auto& Data : this->ExitStateMap)
	{
		Component->SetInteractionState(Data.Key, Data.Value);
	}
}

#if WITH_EDITOR
TArray<FString> UInteractionStateNode::GetInteractionNames() const
{
	TArray<FString> Names;

	this->SearchActor.LoadSynchronous();

	if (SearchActor.Get())
	{
		Names.Append(UInteractableComponent::GetActorInteractions(SearchActor.Get()));
	}

	return Names;
}

void UInteractionStateNode::PostEditChangeProperty(FPropertyChangedEvent& Event)
{
	Super::PostEditChangeProperty(Event);
}
#endif