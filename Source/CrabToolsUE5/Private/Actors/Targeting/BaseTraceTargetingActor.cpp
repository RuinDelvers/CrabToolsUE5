#include "Actors/Targeting/BaseTraceTargetingActor.h"
#include "Actors/Targeting/ITargeter.h"
#include "Components/MouseOverComponent.h"
#include "Logging/MessageLog.h"

#define LOCTEXT_NAMESPACE "BaseTraceTargetingActor"

ABaseTraceTargetingActor::ABaseTraceTargetingActor()
{
	this->PrimaryActorTick.bCanEverTick = true;
	this->BlockedTargetError = LOCTEXT("BlockedTargetError", "Target blocked");
	this->InvalidTargetError = LOCTEXT("InvalidTargetError", "Invalid Target");
}

void ABaseTraceTargetingActor::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	switch (this->PointSource)
	{
		case ETraceTargetingPointSource::TARGETER:
			this->SetActorTickEnabled(true);
			break;
		case ETraceTargetingPointSource::MOUSE_OVER:
			this->SetActorTickEnabled(false);
			if (auto Comp = ITargeterInterface::Execute_GetMouseOver(this->GetUsingActorNative()))
			{
				Comp->OnMouseOverTick.AddDynamic(this, &ABaseTraceTargetingActor::HandleMouseOver);
			}
			else
			{
				FMessageLog Log("PIE");
				Log.Error(
					FText::FormatNamed(
						LOCTEXT("MissingMouseOverError", "MouseOverComponent could not be found for {Targeting} used by {Actor}"),
						TEXT("Targeting"), FText::FromString(this->GetName()),
						TEXT("Actor"), FText::FromString(this->GetUsingActorNative()->GetName())));
			}
			break;
	}
}

void ABaseTraceTargetingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (this->PointSource == ETraceTargetingPointSource::TARGETER)
	{
		this->TargetLocation = ITargeterInterface::Execute_GetEndPoint(this->GetUsingActorNative());
		this->GoalActor = ITargeterInterface::Execute_GetTracedActor(this->GetUsingActorNative());
		this->RouteAction();
	}
}

void ABaseTraceTargetingActor::RouteAction()
{
	this->InvalidateTargetData();

	switch (this->Action)
	{
		case ETraceTargetingAction::SEARCH:
		{
			this->HandleTrace(this->TracedTarget);

			if (IsValid(this->TracedTarget.TargetActor))
			{
				this->UpdateTraces(this->TracedTarget);
			}
			else
			{
				this->OnValidateTargeting.Broadcast(
					false,
					this->InvalidTargetError);
			}
		}
		case ETraceTargetingAction::LINE_OF_SIGHT: 
		{
			this->HandleLineOfSight(this->TracedTarget);

			if (!IsValid(this->TracedTarget.TargetActor) || this->TracedTarget.TargetActor == this->GoalActor)
			{
				FTargetingData NewData;
				NewData.TargetActor = this->GoalActor;
				NewData.TargetLocation = this->TargetLocation;
				
				this->UpdateTraces(NewData);
			}
			else
			{
				this->OnValidateTargeting.Broadcast(
					false,
					this->BlockedTargetError);
			}			
		}
	}
}

void ABaseTraceTargetingActor::HandleMouseOver(UMouseOverComponent* Comp)
{
	if (Comp->HasValidLocation())
	{
		this->TargetLocation = Comp->GetLocation();
		this->GoalActor = Comp->GetMouseOverActor();		
		this->RouteAction();
	}
	else
	{
		this->TargetLocation = this->GetActorLocation();
		this->GoalActor = nullptr;
		this->InvalidateTargetData();
	}	
}

void ABaseTraceTargetingActor::AddDirectTarget_Implementation(const FTargetingData& TargetData)
{
	ITargetingControllerInterface::Execute_AddTarget(this, TargetData);
}

void ABaseTraceTargetingActor::InvalidateTargetData()
{
	this->TracedTarget = FTargetingData();
}

void ABaseTraceTargetingActor::Confirm_Implementation()
{
	Super::Confirm_Implementation();
	if (this->Data.Num() > 0)
	{
		if (this->PointSource == ETraceTargetingPointSource::MOUSE_OVER)
		{
			if (auto Comp = ITargeterInterface::Execute_GetMouseOver(this->GetUsingActorNative()))
			{
				Comp->OnMouseOverTick.RemoveAll(this);
			}
		}
	}
}

void ABaseTraceTargetingActor::PushTarget_Implementation()
{
	if (IsValid(this->TracedTarget.TargetActor))
	{
		ITargetingControllerInterface::Execute_AddTarget(this, this->TracedTarget);
	}
}

void ABaseTraceTargetingActor::UpdateTraces(const FTargetingData& InData)
{
	this->TracedTarget = InData;

	FText Reason;
	bool IsValid = this->ApplyFilters(InData, Reason);

	if (!IsValid)
	{
		this->InvalidateTargetData();
	}

	this->OnValidateTargeting.Broadcast(IsValid, Reason);
}

void ABaseTraceTargetingActor::IgnoreActors_Implementation(TArray<AActor*>& IgnoredActors)
{
	if (this->bIgnoreSelf)
	{
		IgnoredActors.Add(this->GetUsingActorNative());
	}
}

#undef LOCTEXT_NAMESPACE
