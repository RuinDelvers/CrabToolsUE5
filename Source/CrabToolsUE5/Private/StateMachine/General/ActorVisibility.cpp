#include "StateMachine/General/ActorVisibility.h"
#include "Utils/UtilsLibrary.h"

void UActorVisibilityNode::Initialize_Inner_Implementation()
{
	FString Address = this->PropertyName.ToString();
	FSMPropertySearch Params = FSMPropertySearch::ObjectProperty(AActor::StaticClass());
	this->PropertyRef = Params.GetProperty<FObjectProperty>(this->GetMachine(), Address);
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
	return this->PropertyRef.GetValue<AActor>();
}

#if WITH_EDITOR
TArray<FString> UActorVisibilityNode::GetPropertyOptions() const
{
	TArray<FString> Props;

	if (auto Outer = UtilsFunctions::GetOuterAs<IStateMachineLike>(this))
	{
		FSMPropertySearch Params = FSMPropertySearch::ObjectProperty(AActor::StaticClass());

		Props.Append(Outer->GetPropertiesOptions(Params));
	}

	Props.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Props;
}
#endif // WITH_EDITOR