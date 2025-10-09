#include "Utils/ActorComponentPattern.h"
#include "Components/ActorComponent.h"

void FActorComponentPattern::FindComponents(TArray<TObjectPtr<UActorComponent>>& Comps) const
{
	for (const auto& Layer : this->Layers)
	{
		Layer.FindComponents(Comps);
	}
}

void FActorComponentPatternLayer::FindComponents(TArray<TObjectPtr<UActorComponent>>& Comps) const
{
	Comps = Comps.FilterByPredicate([this](const TObjectPtr<UActorComponent>& Comp)
		{
			return this->Matches(Comp);
		});
}

bool FActorComponentPatternLayer::Matches(UActorComponent* Comp) const
{
	if (Comp)
	{
		switch (this->Type)
		{
			case EActorComponentPatternLayerType::CLASS:
				return Comp->IsA(this->ComponentClass);
			case EActorComponentPatternLayerType::TAGS:
				for (const auto& Tag : this->RequiredTags)
				{
					if (!Comp->ComponentTags.Contains(Tag))
					{
						return false;
					}
				}
				return true;
			default: return false;
		}
	}

	return false;
}

UActorComponent* UActorComponentPatternRef::FindComponent(AActor* Actor) const
{
	if (IsValid(Actor))
	{
		TArray<TObjectPtr<UActorComponent>> Comps;

		for (const auto& Comp : Actor->GetComponents())
		{
			Comps.Add(Comp);
		}

		this->Pattern.FindComponents(Comps);

		if (Comps.Num() > 0)
		{
			return Comps[0];
		}
		else
		{
			return nullptr;
		}
	}
	else
	{
		return nullptr;
	}
}

UActorComponent* UActorComponentPatternRef::FindComponentByClass(AActor* Actor, TSubclassOf<UActorComponent> CompClass) const
{
	if (IsValid(Actor))
	{
		TArray<TObjectPtr<UActorComponent>> Comps;

		for (const auto& Comp : Actor->GetComponents())
		{
			Comps.Add(Comp);
		}

		Comps = Comps.FilterByPredicate([CompClass](const TObjectPtr<UActorComponent>& C)
			{
				return C->IsA(CompClass);
			});

		if (!this->Pattern.IsEmpty())
		{
			this->Pattern.FindComponents(Comps);
		}

		if (Comps.Num() > 0)
		{
			return Comps[0];
		}
		else
		{
			return nullptr;
		}
	}
	else
	{
		return nullptr;
	}
}