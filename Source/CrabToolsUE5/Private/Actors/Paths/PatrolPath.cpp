#include "Actors/Paths/PatrolPath.h"
#include "Components/BillboardComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SplineComponent.h"
#include "UObject/ObjectSaveContext.h"

APatrolPath::APatrolPath(): bIsCycle(true)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	auto SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	this->SetRootComponent(SceneComponent);
	this->RootComponent->SetMobility(EComponentMobility::Static);

	// Setup Icon Display in the Editor
	#if WITH_EDITORONLY_DATA
		this->EditorSprite = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("EditorSprite"));

		ConstructorHelpers::FObjectFinderOptional<UTexture2D> Icon(
			TEXT("/CrabToolsUE5/Icons/PatrolPathIcon.PatrolPathIcon"));
		if (EditorSprite)
		{
			this->EditorSprite->Sprite = Icon.Get();
			this->EditorSprite->bHiddenInGame = true;
			this->EditorSprite->SetupAttachment(this->RootComponent);
			this->EditorSprite->SetRelativeScale3D_Direct(FVector(0.4f, 0.4f, 0.4f));
			this->EditorSprite->SetRelativeLocation_Direct(50 * FVector::UpVector);
			this->EditorSprite->SetSimulatePhysics(false);
		}
		
		this->PathSpline = CreateEditorOnlyDefaultSubobject<USplineComponent>(TEXT("PathDisplay"));

		if (PathSpline)
		{
			this->PathSpline->SetClosedLoop(this->bIsCycle);
			this->PathSpline->bHiddenInGame = true;
			this->PathSpline->SetVisibility(false);
			this->PathSpline->bDrawDebug = false;
			this->PathSpline->SetupAttachment(EditorSprite);
			this->PathSpline->SetSimulatePhysics(false);
		}

	#endif

	this->SetCanBeDamaged(false);
}

FVector APatrolPath::FindClosestPoint(AActor* Patroller)
{
	if (Patroller == nullptr) {
		return this->GetActorLocation();
	}

	FVector Goal = this->GetActorLocation();
	float Dist = std::numeric_limits<float>::infinity();

	for (auto& p : this->Data)
	{
		float DSquared = FVector::DistSquared(p.Point, Patroller->GetActorLocation());
		if (DSquared < Dist)
		{
			Goal = p.Point;
			Dist = DSquared;
		}
	}
	return Goal;
}

int APatrolPath::FindClosestIndex(AActor* Patroller) {
	if (Patroller == nullptr)
	{
		return -1;
	} 
	else if (this->Num() == 0)
	{
		return -1;
	}

	int Goal = 0;
	float Dist = std::numeric_limits<float>::infinity();

	for (int i = 0; i < this->Data.Num(); ++i)
	{
		auto p = this->Get(i);
		float DSquared = FVector::DistSquared(p, Patroller->GetActorLocation());

		if (DSquared < Dist) {
			Goal = i;
			Dist = DSquared;
		}
	}

	return Goal;
}

FVector APatrolPath::Get(int i)
{
	// To enable negative indexing.
	i = i % this->Data.Num();

	return this->Data[i].Point + this->GetActorLocation();
}

#if WITH_EDITOR
void APatrolPath::ToggleDisplay()
{
	this->InitArrows();
	this->PathSpline->SetVisibility(this->IsSelected());
}

void APatrolPath::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	bool bUpdateSpline1 = PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(FPatrolPathData, Point);
	bool bUpdateSpline2 = PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(APatrolPath, Data);

	if (bUpdateSpline1 || bUpdateSpline2)
	{
		this->InitArrows();
		this->PathSpline->SetVisibility(this->IsSelected());
	}
	else if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(APatrolPath, bIsCycle))
	{
		this->PathSpline->SetClosedLoop(this->bIsCycle);
	}
}

void APatrolPath::InitArrows()
{
	TArray<FVector> Points;
	for (int i = 0; i < this->Data.Num(); ++ i) { Points.Add(this->Get(i)); }

	this->PathSpline->SetSplineWorldPoints(Points);

	for (int i = 0; i < this->Num(); ++i)
	{
		this->PathSpline->SetTangentAtSplinePoint(i, FVector::Zero(), ESplineCoordinateSpace::World);
	}

	this->EditorSprite->RegisterComponent();
	this->PathSpline->RegisterComponent();
}

void APatrolPath::PreSave(FObjectPreSaveContext SaveContext)
{
	this->PathSpline->SetVisibility(false);
	Super::PreSave(SaveContext);
}

void APatrolPath::PostSaveRoot(FObjectPostSaveRootContext SaveContext)
{
	if (this->IsSelected())
	{
		this->PathSpline->SetVisibility(true);
	}

	Super::PostSaveRoot(SaveContext);
}

#endif

int UCyclicPathSequence::Target_Implementation() const
{
	if (Path)
	{
		auto PointCount = Path->Num();
		
		if (PointCount > 0)
		{
			return this->CurrentIndex;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

int UCyclicPathSequence::Step_Implementation()
{
	int Increment = this->bDirection ? 1 : -1;

	if (Path->IsCycle())
	{
		if (this->bDirection)
		{
			if (this->CurrentIndex == Path->Num() - 1)
			{
				this->bDirection = !this->bDirection;
				Increment = -Increment;
			}
		}
		else
		{
			if (this->CurrentIndex == 0)
			{
				this->bDirection = !this->bDirection;
				Increment = -Increment;
			}
		}

		this->CurrentIndex = this->CurrentIndex + Increment;
	}
	else
	{
		this->CurrentIndex = (this->CurrentIndex + Increment) % Path->Num();
	}

	return this->CurrentIndex;
}

void UCyclicPathSequence::SetDirection(int Index)
{
	if (IsValid(this->Path))
	{
		Index = Index % this->Path->Num();

		if (Index >= this->CurrentIndex)
		{
			this->bDirection = true;
		}
		else
		{
			this->bDirection = true;
		}
	}
	else
	{
		this->bDirection = true;
	}
}

void UCyclicPathSequence::Reset()
{
	this->CurrentIndex = StartIndex;
}

FVector UPatrolPathLibrary::GetTarget(const FPatrolPathState& State)
{
	return State.Point();
}

int FPatrolPathState::Target() const
{
	if (this->Sequence)
	{
		return this->Sequence->Target();
	}
	else
	{
		return 0;
	}
}

FVector FPatrolPathState::Point() const
{
	if (this->Sequence)
	{
		return this->Sequence->CurrentPoint();
	}
	else
	{
		return FVector::Zero();
	}
}

int FPatrolPathState::Points() const
{
	if (this->Sequence && this->Sequence->GetPath())
	{
		return this->Sequence->GetPath()->Num();
	}
	else
	{
		return 0;
	}
}

void FPatrolPathState::Reset()
{
	if (this->Sequence)
	{
		this->Sequence->Reset();
	}
}

int FPatrolPathState::Step()
{
	if (this->Sequence)
	{
		return this->Sequence->Step();
	}
	else
	{
		return 0;
	}
}

FPatrolPathState::operator bool() const
{
	return this->Sequence && this->Sequence->GetPath() && this->Sequence->GetPath()->Num() >= 2;
}

int UPathSequence::Target_Implementation() const
{
	return 0;
}

int UPathSequence::Step_Implementation()
{
	return 0;
}

FVector UPathSequence::Point(int Index) const
{
	if (this->Path)
	{
		return this->Path->Get(Index);
	}
	else
	{
		return FVector::Zero();
	}
}

FVector UPathSequence::CurrentPoint() const
{
	if (this->Path)
	{
		return this->Path->Get(this->Target());
	}
	else
	{
		return FVector::Zero();
	}
}