#include "Actors/DynamicActorSpawner.h"
#include "UObject/ObjectSaveContext.h"
#include "Components/BillboardComponent.h"

// Sets default values
ADynamicActorSpawner::ADynamicActorSpawner()
{
	this->SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot")));
	this->PrimaryActorTick.bCanEverTick = false;
	this->PrimaryActorTick.bStartWithTickEnabled = false;

	#if WITH_EDITORONLY_DATA
		this->EditorSprite = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("EditorSprite"));

		static ConstructorHelpers::FObjectFinderOptional<UTexture2D> Icon(
			TEXT("/CrabToolsUE5/Icons/ActorSpawnerIcon.ActorSpawnerIcon"));

		if (EditorSprite)
		{
			this->EditorSprite->Sprite = Icon.Get();
			this->EditorSprite->bHiddenInGame = true;
			this->EditorSprite->SetupAttachment(this->RootComponent);
			this->EditorSprite->SetRelativeScale3D_Direct(FVector(0.4f, 0.4f, 0.4f));
			this->EditorSprite->SetRelativeLocation_Direct(50 * FVector::UpVector);
			this->EditorSprite->SetSimulatePhysics(false);
		}

	#endif // WITH_EDITORONLY_DATA
}

void ADynamicActorSpawner::BeginPlay()
{
	#if WITH_EDITORONLY_DATA
		
	#endif
}

void ADynamicActorSpawner::SpawnActor()
{
	this->ActorClass.LoadSynchronous();

	FTransform Transform = this->GetActorTransform();

	this->GetWorld()->SpawnActor(this->ActorClass.Get(), &Transform);
}

void ADynamicActorSpawner::Destroyed()
{
	Super::Destroyed();

	if (IsValid(this->PreviewActor))
	{
		this->PreviewActor->Destroy();
		this->PreviewActor = nullptr;
	}
}

#if WITH_EDITOR

void ADynamicActorSpawner::ClearPreviews()
{
	if (this->PreviewActor)
	{
		this->PreviewActor->Destroy();
	}
}

void ADynamicActorSpawner::ToggleDisplay()
{
	if (this->PreviewActor)
	{
		this->PreviewActor->Destroy();
		this->PreviewActor = nullptr;

		this->EditorSprite->SetVisibility(true);
	}
	else
	{
		this->ActorClass.LoadSynchronous();
		if (this->ActorClass)
		{
			FTransform Transform = this->GetActorTransform();
			FActorSpawnParameters Params;

			Params.ObjectFlags = RF_Transient;
			Params.bTemporaryEditorActor = true;

			this->PreviewActor = this->GetWorld()->SpawnActor(this->ActorClass.Get(), &Transform, Params);
			this->PreviewActor->SetActorEnableCollision(false);

			this->EditorSprite->SetVisibility(false);
		}
	}
}

void ADynamicActorSpawner::PostEditChangeProperty(struct FPropertyChangedEvent& Event)
{
	Super::PostEditChangeProperty(Event);	

	if (this->ActorClass)
	{
		this->ActorClass.LoadSynchronous();
	}
	else
	{
		
	}
}
#endif // WITH_EDITOR