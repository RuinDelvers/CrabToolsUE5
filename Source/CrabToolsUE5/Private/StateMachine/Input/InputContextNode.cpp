#include "StateMachine/Input/InputContextNode.h"
#include "EnhancedInputSubsystems.h"



void UInputContextNode::Initialize_Inner_Implementation() {
	Super::Initialize_Inner_Implementation();

	this->PawnOwner = Cast<APawn>(this->GetMachine()->GetActorOwner());
}


void UInputContextNode::SetActive_Inner_Implementation(bool bNewActive)
{
	if (auto PC = this->PawnOwner->GetLocalViewingPlayerController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (bNewActive)
			{
				if (this->bClearContextOnEnter)
				{
					Subsystem->ClearAllMappings();
				}
				else
				{
					for (const auto& Mapping : this->ContextsToRemove)
					{
						Subsystem->RemoveMappingContext(Mapping);
					}
				}

				for (const auto& Mapping : this->ContextsToAdd)
				{
					Subsystem->AddMappingContext(Mapping.Mapping, Mapping.Priority, Mapping.Options);
				}
			}
			else
			{
				if (this->bClearContextOnExit)
				{
					Subsystem->ClearAllMappings();
				}
			}
		}
	}	
}