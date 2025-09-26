#include "Components/MouseOverComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "Slate/SGameLayerManager.h"
#include "Blueprint/DragDropOperation.h"


UMouseOverComponent::UMouseOverComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UMouseOverComponent::BeginPlay()
{
	Super::BeginPlay();

	this->PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	Params.AddIgnoredActor(this->GetOwner());

	if (auto LocalPlayer = PlayerController->GetLocalPlayer())
	{
		LocalPlayer->ViewportClient->Viewport->ViewportResizedEvent.AddUObject(this, &UMouseOverComponent::OnViewportResized);
	}

	this->UpdateViewportData();
}

void UMouseOverComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	this->DoTrace();
}

void UMouseOverComponent::DoTrace()
{
	FVector Position;
	FVector Direction;

	auto bSuccess = this->bOverridePosition && !this->bIsMousingUI
		? this->PlayerController->DeprojectScreenPositionToWorld(
			this->OverrideLocation.X,
			this->OverrideLocation.Y,
			Position,
			Direction)
		: this->PlayerController->DeprojectMousePositionToWorld(Position, Direction);

	if (bSuccess)
	{
		FVector EndPoint = Position + MaxTraceDistance * Direction;

		#if WITH_EDITORONLY_DATA
			if (this->bDrawDebugLine)
			{
				DrawDebugLine(this->GetWorld(), Position, EndPoint, FColor(1, 1, 1), false, 0.1f, 0, 5.0f);
			}
		#endif //WITH_EDITORONLY_DATA

		if (this->bDoMultiTrace)
		{
			TArray<FHitResult> Results;
			bSuccess = this->GetWorld()->LineTraceMultiByChannel(
				Results,
				Position,
				EndPoint,
				Channel,
				this->Params);			

			if (bSuccess)
			{
				for (const auto& CheckResult : Results)
				{
					if (this->ProcessResult(CheckResult))
					{
						break;
					}
				}
			}
			else
			{
				this->ReplaceActors(nullptr);
			}
		}
		else
		{
			FHitResult CheckResult;
			bSuccess = this->GetWorld()->LineTraceSingleByChannel(
				CheckResult,
				Position,
				EndPoint,
				Channel,
				this->Params);

			if (bSuccess)
			{
				this->ProcessResult(CheckResult);
			}
			else
			{
				this->ReplaceActors(nullptr);
			}
		}
	}
	else
	{
		this->ReplaceActors(nullptr);
	}

	this->UpdateMousePointActors();
}

void UMouseOverComponent::SetOverridePosition(const FPointerEvent& Ev, bool bShouldUpdateTraces)
{
	// Call this, since the call back for resizing has issues for some reason.
	this->UpdateViewportData();
	// This is based off code from the AbsoluteToViewport node in the SlateBlueprintLibrary.
	auto Position = this->ViewportGeometry.AbsoluteToLocal(Ev.GetScreenSpacePosition());
	this->OverrideLocation = (Position / ViewportGeometry.GetLocalSize()) * ViewportSize;
	
	this->bOverridePosition = true;

	if (bShouldUpdateTraces)
	{
		this->DoTrace();
	}
}

void UMouseOverComponent::UpdateViewportData()
{
	if (auto LocalPlayer = PlayerController->GetLocalPlayer())
	{
		if (LocalPlayer->ViewportClient)
		{
			auto Manager = LocalPlayer->ViewportClient->GetGameLayerManager();
			ViewportGeometry = Manager->GetViewportWidgetHostGeometry();
			LocalPlayer->ViewportClient->GetViewportSize(ViewportSize);
		}
	}
}

FTransform UMouseOverComponent::GetPlacementTransform() const
{
	return FTransform(
		FRotationMatrix::MakeFromZ(this->Result.ImpactNormal).Rotator(),
		this->Result.ImpactPoint);
}

void UMouseOverComponent::ClearOverride()
{
	this->OverrideLocation = FVector2D::ZeroVector;
	this->bOverridePosition = false;
}

void UMouseOverComponent::SetComponentTickEnabled(bool bNewEnabled)
{
	this->bShouldTick = bNewEnabled;
	this->UpdateTickingState();
}

void UMouseOverComponent::SetIsMousingUI(bool bNewMousing)
{
	this->bIsMousingUI = bNewMousing;

	this->UpdateTickingState();
}

void UMouseOverComponent::UpdateTickingState()
{
	// ShouldTick && (Ticking -> Mousing)
	bool bTick = 
		this->bShouldTick
		&& (!this->bIsMousingUI || this->bTickWhenMousingUI);
	
	UActorComponent::SetComponentTickEnabled(bTick);

	if (!bTick && this->bClearTargetOnTickOff)
	{
		this->ReplaceActors(nullptr);
		this->UpdateMousePointActors();
	}
}


void UMouseOverComponent::PlaceActorAtLocation(AActor* PlacedActor, bool bUseNormal) const
{
	if (IsValid(PlacedActor) && IsValid(this->MouseOverActor))
	{
		FTransform Transform(
			bUseNormal ? this->Result.ImpactNormal.Rotation() : PlacedActor->GetActorRotation(),
			this->Result.ImpactPoint);

		PlacedActor->SetActorTransform(Transform);
	}
}

bool UMouseOverComponent::IsOnNavMesh() const
{
	if (IsValid(this->MouseOverActor))
	{
		UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(this->GetWorld());

		FNavLocation NavLoc;
		return NavSys->ProjectPointToNavigation(this->Result.ImpactPoint, NavLoc);
	}
	else
	{
		return false;
	}
}

bool UMouseOverComponent::IsWithinDistance(float Distance) const
{
	if (this->HasValidLocation())
	{
		UE_LOG(LogTemp, Warning, TEXT("Distance = %f"), FVector::Dist(this->Result.ImpactPoint, this->GetOwner()->GetActorLocation()));
		return FVector::DistSquared(this->Result.ImpactPoint, this->GetOwner()->GetActorLocation()) < Distance * Distance;
	}
	else
	{
		return false;
	}
}

bool UMouseOverComponent::ProcessResult(const FHitResult& CheckResult)
{
	if (Result.bBlockingHit)
	{
		this->Result = CheckResult;
		this->ReplaceActors(this->Result.GetActor());
		return true;
	}
	else if (IsValid(CheckResult.GetActor()))
	{
		this->Result = CheckResult;
		if (this->ValidateActor(CheckResult.GetActor()))
		{			
			this->ReplaceActors(Result.GetActor());
			return true;
		}
		else
		{
			this->ReplaceActors(nullptr);
			return false;
		}
	}
	else
	{
		this->ReplaceActors(nullptr);
		return false;
	}
}

void UMouseOverComponent::ReplaceActors(AActor* NewActor)
{
	if (this->MouseOverActor != NewActor)
	{
		if (IsValid(this->MouseOverActor))
		{
			this->OnMouseOff(this->MouseOverActor);
		}

		this->MouseOverActor = NewActor;

		if (IsValid(NewActor))
		{
			this->OnMouseOn(NewActor);
		}

		this->OnMouseOverActor.Broadcast(NewActor);
	}
}

void UMouseOverComponent::UpdateMousePointActors()
{
	if (this->HasValidLocation())
	{
		for (const auto& Actor : this->MousePointActors)
		{
			Actor->SetActorTransform(this->GetPlacementTransform());
			Actor->SetActorHiddenInGame(false);
		}
	}
	else
	{
		FVector ActorLocation = this->GetOwner()->GetActorLocation();

		for (const auto& Actor : this->MousePointActors)
		{
			Actor->SetActorLocation(ActorLocation);
			Actor->SetActorHiddenInGame(true);
		}
	}
}

AActor* UMouseOverComponent::AddMousePointActor(TSubclassOf<AActor> ActorClass)
{
	auto ActorLocation = this->GetOwner()->GetActorLocation();
	auto NewActor = this->GetWorld()->SpawnActor(
		ActorClass, 
		this->HasValidLocation() ? &this->Result.ImpactPoint : &ActorLocation);
	NewActor->SetHidden(!this->HasValidLocation());

	this->MousePointActors.Add(NewActor);

	return NewActor;
}

void UMouseOverComponent::RemoveMousePointActor(AActor* ActorToRemove)
{
	this->MousePointActors.Remove(ActorToRemove);
	ActorToRemove->Destroy();
}

void UMouseOverComponent::ClearMousePointActors()
{
	for (const auto& Actor : this->MousePointActors)
	{
		Actor->Destroy();
	}

	this->MousePointActors.Empty();
}

void UMouseOverComponent::SetMousePointActorsHidden(bool bHidden)
{
	for (const auto& Actor : this->MousePointActors)
	{
		Actor->SetHidden(bHidden);
	}
}

void UMouseOverComponent::OnViewportResized(FViewport* Viewport, uint32 Index)
{
	this->UpdateViewportData();
}

#if WITH_EDITOR
void UMouseOverComponent::DisplayDebug()
{
	if (IsValid(this->MouseOverActor))
	{
		DrawDebugSphere(this->GetWorld(), this->Result.ImpactPoint, 10, 12, FColor(0, 1, 0));
	}
}
#endif