#include "StateMachine/Transitions/EventComboTransition.h"
#include "Utils/UtilsLibrary.h"

bool UEventComboTransitionCondition::Check() const
{
	for (const auto& Data : this->Combos)
	{
		if (Data.IsActive())
		{
			return true;
		}
	}

	return false;
}

void UEventComboTransitionCondition::Event_Implementation(FName InEvent)
{
	for (auto& Data : this->Combos)
	{
		Data.Activate(InEvent);
	}
}

void UEventComboTransitionCondition::Exit_Implementation()
{
	for (auto& Data : this->Combos)
	{
		Data.Exit();
	}
}

bool UEventComboTransitionDataCondition::Check(UObject* Data) const
{
	for (const auto& DataPoint : this->Combos)
	{
		if (DataPoint.IsActive())
		{
			return true;
		}
	}

	return false;
}

void UEventComboTransitionDataCondition::Event_Implementation(FName InEvent)
{
	for (auto& Data : this->Combos)
	{
		Data.Activate(InEvent);
	}
}

void UEventComboTransitionDataCondition::Exit_Implementation()
{
	for (auto& Data : this->Combos)
	{
		Data.Exit();
	}
}

#if WITH_EDITOR
TArray<FString> UEventComboTransitionCondition::GetEventOptions() const
{
	TArray<FString> Names;

	if (auto StateLike = UtilsFunctions::GetOuterAs<IStateTransitionLike>(this))
	{
		Names.Append(StateLike->GetSourceEventOptions());
	}

	return Names;
}

TArray<FString> UEventComboTransitionDataCondition::GetEventOptions() const
{
	TArray<FString> Names;

	if (auto StateLike = UtilsFunctions::GetOuterAs<IStateLike>(this))
	{
		Names.Append(StateLike->GetEventOptions());
	}

	return Names;
}
#endif


bool FEventComboNodeData::IsActive() const
{
	for (const auto& Data : this->Events)
	{
		if (!Data.Value.bActive)
		{
			return false;
		}
	}

	return true;
}

void FEventComboNodeData::Reset()
{
	for (auto& Point : this->Events)
	{
		Point.Value.bActive = false;
	}
}

void FEventComboNodeDataPoint::Activate()
{
	this->bActive = true;

	switch (this->Type)
	{
		case EEventComboType::FRAME:
			if (GEngine)
			{
				this->TimeUp = GEngine->GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
					{
						this->TimeUpCallback();
					});
			}
			break;

		case EEventComboType::TIME:
			if (GEngine)
			{
				GEngine->GetWorld()->GetTimerManager().SetTimer(this->TimeUp, [this]()
					{
						this->TimeUpCallback();
					},
					this->Time,
					false);
			}
			break;
	}
}

void FEventComboNodeDataPoint::Deactivate()
{
	this->bActive = false;

	switch (this->Type)
	{
		case EEventComboType::FRAME:
			if (GEngine)
			{
				GEngine->GetWorld()->GetTimerManager().ClearTimer(this->TimeUp);
			}
			break;

		case EEventComboType::TIME:
			if (GEngine)
			{
				GEngine->GetWorld()->GetTimerManager().ClearTimer(this->TimeUp);
			}
			break;
	}
}

void FEventComboNodeDataPoint::TimeUpCallback()
{
	this->bActive = false;
}

void FEventComboNodeData::Exit()
{
	for (auto& Point : this->Events)
	{
		if (Point.Value.Type == EEventComboType::ON_EXIT && Point.Value.bActive)
		{
			Point.Value.Activate();
		}
	}
}

bool FEventComboNodeData::Activate(FName EName)
{
	if (auto Point = this->Events.Find(EName))
	{
		Point->Activate();
		return true;
	}
	else
	{
		return false;
	}
}

bool FEventComboNodeData::Deactivate(FName EName)
{
	if (auto Point = this->Events.Find(EName))
	{
		Point->Deactivate();

		return true;
	}
	else
	{
		return false;
	}
}