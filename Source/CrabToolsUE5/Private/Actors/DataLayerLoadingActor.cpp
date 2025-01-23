#include "Actors/DataLayerLoadingActor.h"
#include "Components/BoxComponent.h"

#include "WorldPartition/WorldPartitionSubsystem.h"
#include "WorldPartition/DataLayer/DataLayerSubsystem.h"

ADataLayerLoadingActor::ADataLayerLoadingActor()
{
	this->SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));

	this->LoaderDetector = CreateDefaultSubobject<UBoxComponent>(TEXT("LoaderDetector"));
	this->PauseDetector = CreateDefaultSubobject<UBoxComponent>(TEXT("PauseDetector"));

	this->LoaderDetector->SetBoxExtent(FVector(50, 50, 50));
	this->LoaderDetector->SetCollisionProfileName("OverlapAll");
	this->LoaderDetector->SetCollisionResponseToAllChannels(ECR_Ignore);
	this->LoaderDetector->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	this->LoaderDetector->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	this->LoaderDetector->SetupAttachment(this->RootComponent);

	this->PauseDetector->SetBoxExtent(FVector(50, 50, 50));
	this->PauseDetector->SetCollisionProfileName("OverlapAll");
	this->PauseDetector->SetCollisionResponseToAllChannels(ECR_Ignore);
	this->PauseDetector->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	this->PauseDetector->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	this->PauseDetector->SetupAttachment(this->RootComponent);

	this->LoaderDetector->OnComponentBeginOverlap.AddDynamic(this, &ADataLayerLoadingActor::OnLoaderOverlap);
	this->PauseDetector->OnComponentBeginOverlap.AddDynamic(this, &ADataLayerLoadingActor::OnPauserOverlap);
}

void ADataLayerLoadingActor::OnLoaderOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if (auto DLSystem = this->GetWorld()->GetDataLayerManager())
	{	
		if (IsValid(this->LoadedLayer))
		{
			DLSystem->SetDataLayerRuntimeState(this->LoadedLayer, EDataLayerRuntimeState::Activated);
		}
	}	
}

void ADataLayerLoadingActor::OnPauserOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (auto WPS = this->GetWorld()->GetSubsystem<UWorldPartitionSubsystem>())
	{

	}
}