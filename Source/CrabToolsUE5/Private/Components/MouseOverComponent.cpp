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
	
	if (this->PawnOwner = Cast<APawn>(this->GetOwner()))
	{
		this->PlayerController = Cast<APlayerController>(this->PawnOwner->GetController());
		PawnOwner->ReceiveControllerChangedDelegate.AddDynamic(this, &UMouseOverComponent::OnControllerChanged);
	}

	Params.AddIgnoredActor(this->GetOwner());

	if (auto LocalPlayer = PlayerController->GetLocalPlayer())
	{
		LocalPlayer->ViewportClient->Viewport->ViewportResizedEvent.AddUObject(this, &UMouseOverComponent::OnViewportResized);
	}

	this->UpdateViewportData();
}

void UMouseOverComponent::OnControllerChanged(APawn* Pawn, AController* OldController, AController* NewController)
{
	this->PlayerController = Cast<APlayerController>(NewController);
}


void UMouseOverComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsValid(this->PlayerController))
	{
		this->DoTrace();
	}
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
		EndPoint = Position + MaxTraceDistance * Direction;

		#if WITH_EDITORONLY_DATA
			if (this->bDrawDebugLine)
			{
				DrawDebugLine(this->GetWorld(), Position, EndPoint, FColor(1, 1, 1), false, 0.1f, 0, 5.0f);
			}
		#endif //WITH_EDITORONLY_DATA

		if (this->bDoMultiTrace)
		{
			bSuccess = this->GetWorld()->LineTraceMultiByChannel(
				this->MultiResults,
				Position,
				this->EndPoint,
				this->Channel,
				this->Params);			

			if (bSuccess)
			{
				FHitResult& FinalResult = this->MultiResults[0];
				bool bDidPass = false;

				for (const auto& CheckResult : this->MultiResults)
				{
					if (this->ProcessResult(CheckResult, true))
					{
						bDidPass = true;
						FinalResult = CheckResult;

						if (CheckResult.bBlockingHit)
						{
							break;
						}
					}

					this->CurrentResultIndex += 1;
				}

				if (bDidPass)
				{
					this->UpdateResult(FinalResult);
				}
				else
				{
					this->ReplaceActors(nullptr);
				}
			}
			else
			{
				this->ReplaceActors(nullptr);
			}
		}
		else
		{
			bSuccess = this->GetWorld()->LineTraceSingleByChannel(
				this->CurrentCheckResult,
				Position,
				this->EndPoint,
				this->Channel,
				this->Params);

			if (bSuccess)
			{
				this->ProcessResult(this->CurrentCheckResult);
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

	this->OnMouseOverTick.Broadcast(this);

	this->EndPoint = FVector::ZeroVector;
	this->CurrentResultIndex = 0;
}

void UMouseOverComponent::SetTraceComplex(bool bTraceComplex)
{
	this->Params.bTraceComplex = bTraceComplex;
}

const FHitResult& UMouseOverComponent::GetCurrentCheckResult() const
{
	if (this->bDoMultiTrace)
	{
		return this->MultiResults[this->CurrentResultIndex];
	}
	else
	{
		return this->CurrentCheckResult;
	}
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
		FRotationMatrix::MakeFromZX(this->Result.ImpactNormal, FVector::ForwardVector).Rotator(),
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
			bUseNormal ? FRotationMatrix::MakeFromZX(this->Result.ImpactNormal, FVector::ForwardVector).Rotator() : PlacedActor->GetActorRotation(),
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
		return FVector::DistSquared(this->Result.ImpactPoint, this->GetOwner()->GetActorLocation()) < Distance * Distance;
	}
	else
	{
		return false;
	}
}

bool UMouseOverComponent::ProcessResult(const FHitResult& CheckResult, bool bIsMulti)
{
	if (CheckResult.bBlockingHit)
	{
		if (this->ValidateActor(CheckResult.GetActor()))
		{
			if (!bIsMulti)
			{
				this->UpdateResult(CheckResult);
			}
			return true;
		}
		else
		{
			if (!bIsMulti)
			{
				this->ReplaceActors(nullptr);
			}
			return false;
		}
	}
	else if (IsValid(CheckResult.GetActor()))
	{
		if (this->ValidateActor(CheckResult.GetActor()))
		{			
			if (!bIsMulti)
			{
				this->UpdateResult(CheckResult);
			}
			return true;
		}
		else
		{
			if (!bIsMulti)
			{
				this->ReplaceActors(nullptr);
			}
			return false;
		}
	}
	else
	{
		if (!bIsMulti)
		{
			this->ReplaceActors(nullptr);
		}
		return false;
	}
}

void UMouseOverComponent::UpdateResult(const FHitResult& CheckResult)
{
	this->Result = CheckResult;
	this->ReplaceActors(this->Result.GetActor());
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
	for (const auto& Actor : this->MousePointActors)
	{
		IMousePointActorInterface::Execute_BeginPlacement(Actor);

		FVector ActorLocation = this->GetOwner()->GetActorLocation();
		FTransform PlaceTransform = this->GetPlacementTransform();
		bool bHasValidLocation = this->HasValidLocation();

		if (Actor->Implements<UMousePointActorInterface>())
		{
			if (bHasValidLocation)
			{
				bool bReadjust = IMousePointActorInterface::Execute_ReadjustActor(Actor);

				if (bReadjust)
				{
					FVector AdjustLocation = PlaceTransform.GetLocation() + (this->Result.Normal * this->AdjustBias);

					if (GetWorld()->FindTeleportSpot(Actor, AdjustLocation, PlaceTransform.Rotator()))
					{
						PlaceTransform.SetLocation(AdjustLocation);
						Actor->SetActorTransform(PlaceTransform);
						Actor->SetActorHiddenInGame(false);
					}
					else
					{
						switch (IMousePointActorInterface::Execute_OnReadjustFailure(Actor))
						{
							case EMousePointActorPlacementAction::HIDDEN:
								Actor->SetActorHiddenInGame(true);
								break;
							case EMousePointActorPlacementAction::HIDDEN_ATTACH:
								Actor->SetActorHiddenInGame(true);
								Actor->SetActorLocation(ActorLocation);
								break;
							case EMousePointActorPlacementAction::PLACE:
								Actor->SetActorHiddenInGame(false);
								Actor->SetActorTransform(PlaceTransform);
								break;
							default: break;
						}
					}					
				}
				else
				{
					Actor->SetActorTransform(PlaceTransform);
					Actor->SetActorHiddenInGame(false);
				}
			}
			else
			{
				switch (IMousePointActorInterface::Execute_OnInvalidLocation(Actor))
				{
					case EMousePointActorPlacementAction::HIDDEN:
						Actor->SetActorHiddenInGame(true);
						break;
					case EMousePointActorPlacementAction::HIDDEN_ATTACH:
						Actor->SetActorHiddenInGame(true);
						Actor->SetActorLocation(ActorLocation);
						break;
					default: break;
				}
			}

			IMousePointActorInterface::Execute_EndPlacement(Actor);
		}
		else
		{
			Actor->SetActorTransform(PlaceTransform);
			Actor->SetActorHiddenInGame(false);
		}
	}
}

AActor* UMouseOverComponent::AddMousePointActor(TSubclassOf<AActor> ActorClass)
{
	if (!ActorClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("None passed to UMouseOverComponent::AddMousePointActor"));
		return nullptr;
	}

	auto ActorLocation = this->GetOwner()->GetActorLocation();
	auto NewActor = this->GetWorld()->SpawnActor(
		ActorClass, 
		this->HasValidLocation() ? &this->Result.ImpactPoint : &ActorLocation);
	NewActor->SetHidden(!this->HasValidLocation());

	if (NewActor->Implements<UMousePointActorInterface>())
	{
		IMousePointActorInterface::Execute_SetMouseOver(NewActor, this);
	}

	this->MousePointActors.Add(NewActor);

	return NewActor;
}

void UMouseOverComponent::AddMousePointActorInstance(AActor* NewActor)
{
	if (!IsValid(NewActor)) { return; }

	NewActor->SetActorLocation(this->HasValidLocation() ? this->Result.ImpactPoint : this->GetOwner()->GetActorLocation());
	NewActor->SetHidden(!this->HasValidLocation());

	this->MousePointActors.Add(NewActor);
}

void UMouseOverComponent::RemoveMousePointActor(AActor* ActorToRemove)
{
	if (IsValid(ActorToRemove))
	{
		this->MousePointActors.Remove(ActorToRemove);
		ActorToRemove->Destroy();
	}
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