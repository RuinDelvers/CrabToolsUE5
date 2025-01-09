#include "Actors/DynamicActorSpawner.h"

// Sets default values
ADynamicActorSpawner::ADynamicActorSpawner()
{
	#if WITH_EDITORONLY_DATA
		this->StaticMeshPreview = CreateEditorOnlyDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshPreview"));
		this->SkeletalMeshPreview = CreateEditorOnlyDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshPreview"));
	#endif // WITH_EDITORONLY_DATA
}

void ADynamicActorSpawner::SpawnActor() const
{
	this->ActorClass.LoadSynchronous();

	FVector Position = this->GetActorLocation();
	this->GetWorld()->SpawnActor(this->ActorClass.Get(), &Position);
}

#if WITH_EDITOR

void ADynamicActorSpawner::ClearPreviews()
{
	this->SkeletalMeshPreview->SetSkeletalMesh(nullptr);
	this->StaticMeshPreview->SetStaticMesh(nullptr);
}

void ADynamicActorSpawner::PostEditChangeProperty(struct FPropertyChangedEvent& Event)
{
	Super::PostEditChangeProperty(Event);

	bool bUpdateCheck1 = Event.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ADynamicActorSpawner, ActorClass);
	bool bUpdateCheck2 = Event.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ADynamicActorSpawner, PreviewType);

	if (bUpdateCheck1 || bUpdateCheck2)
	{
		this->ClearPreviews();
		this->ActorClass.LoadSynchronous();

		switch (this->PreviewType)
		{
			case ESpawnerPreviewType::STATIC_MESH: 
				if (auto Comp = this->GetActorObject()->GetComponentByClass<UStaticMeshComponent>())
				{
					this->StaticMeshPreview->SetStaticMesh(Comp->GetStaticMesh());
				}
				break;
			case ESpawnerPreviewType::SKELETAL_MESH:
				if (auto Comp = this->GetActorObject()->GetComponentByClass<USkeletalMeshComponent>())
				{
					this->SkeletalMeshPreview->SetSkeletalMesh(Comp->GetSkeletalMeshAsset());
				}
				break;
		}
	}
}
#endif // WITH_EDITOR