#include "StateMachine/General/ActorVisibility.h"
#include "Utils/UtilsLibrary.h"

UActorVisibilityNode::UActorVisibilityNode()
{
	this->Property = CreateDefaultSubobject<UStateMachineProperty>(TEXT("TargetActor"));
	this->Property->Params = FSMPropertySearch::ObjectProperty(AActor::StaticClass());
}

void UActorVisibilityNode::Initialize_Inner_Implementation()
{
	
}

void UActorVisibilityNode::Enter_Inner_Implementation()
{
	auto Actor = this->GetActor();

	if (IsValid(Actor))
	{
		Actor->SetActorHiddenInGame(!this->bShowActor);
	}
}

void UActorVisibilityNode::Exit_Inner_Implementation()
{
	if (this->bRevertOnExit)
	{
		auto Actor = this->GetActor();

		if (IsValid(Actor))
		{
			Actor->SetActorHiddenInGame(this->bShowActor);
		}
	}
}

AActor* UActorVisibilityNode::GetActor() const
{
	return this->Property->GetProperty().GetValue<AActor>();
}
