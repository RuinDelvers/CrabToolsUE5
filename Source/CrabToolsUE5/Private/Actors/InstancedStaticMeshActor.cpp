#include "Actors/InstancedStaticMeshActor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"

AInstancedStaticMeshActor::AInstancedStaticMeshActor()
{
	this->InstanceManager = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstanceManager"));
	this->InstanceManager->SetMobility(EComponentMobility::Static);

	this->SetRootComponent(this->InstanceManager.Get());
}

void AInstancedStaticMeshActor::PostActorCreated()
{
	this->RespawnInstances();
}

void AInstancedStaticMeshActor::RespawnInstances()
{
	this->InstanceManager->ClearInstances();

	if (!this->InstanceManager->GetStaticMesh()) { return; }

	this->InstanceManager->AddInstances(
		this->InstanceTransforms, false, this->bUseWorldTransforms, true);
}

#if WITH_EDITOR

void AInstancedStaticMeshActor::PostEditChangeProperty(FPropertyChangedEvent& Event)
{
	Super::PostEditChangeProperty(Event);

	this->RespawnInstances();
}

void AInstancedStaticMeshActor::SetInstances(TArray<AStaticMeshActor*> MeshActors)
{
	if (MeshActors.Num() == 0) { return; }

	// Get the details from the first actor.
	UStaticMesh* Mesh = MeshActors[0]->GetStaticMeshComponent()->GetStaticMesh();

	if (!Mesh) { return; }

	this->InstanceManager->SetStaticMesh(Mesh);

	int index = 0;
	for (auto& Mats : MeshActors[0]->GetStaticMeshComponent()->GetMaterials())
	{
		this->InstanceManager->SetMaterial(index, Mats);
		index += 1;
	}

	FName NewFolderPath = MeshActors[0]->GetFolderPath();

	// Computer the center location of all the meshes as our new location.
	FVector Center;

	for (auto& Actor : MeshActors)
	{
		if (Actor) Center += Actor->GetActorLocation();
	}

	Center /= MeshActors.Num();

	this->SetActorLocation(Center);

	this->InstanceTransforms.Empty(MeshActors.Num());

	// Computer Total transforms of each instance.
	for (auto& Actor : MeshActors)
	{
		FTransform ATrans = Actor->GetActorTransform();
		ATrans.SetLocation(ATrans.GetLocation() - Center);

		this->InstanceTransforms.Add(ATrans);

		Actor->Destroy();
	}

	this->SetFolderPath(NewFolderPath);
	this->RespawnInstances();
}

void AInstancedStaticMeshActor::BreakOutInstances()
{
	for (int i = 0; i < this->InstanceManager->GetInstanceCount(); ++i)
	{
		FActorSpawnParameters Params;
		FTransform Trans;

		this->InstanceManager->GetInstanceTransform(i, Trans, true);

		auto SMActor = Cast<AStaticMeshActor>(this->GetWorld()->SpawnActor(
			AStaticMeshActor::StaticClass(),
			&Trans,
			Params));

		SMActor->GetStaticMeshComponent()->SetStaticMesh(this->InstanceManager->GetStaticMesh());

		int index = 0;
		for (auto& Mat : this->InstanceManager->GetMaterials())
		{
			SMActor->GetStaticMeshComponent()->SetMaterial(index, Mat);
			index += 1;
		}

		SMActor->SetFolderPath(this->GetFolderPath());
	}

	this->Destroy();
}

#endif