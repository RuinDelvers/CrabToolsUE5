#pragma once

#include "CoreMinimal.h"
#include "LevelSequenceActor.h"
#include "SplineLevelSequenceActor.generated.h"

class USplineComponent;


UCLASS(Blueprintable)
class CRABTOOLSUE5_API ASplineLevelSequenceActor : public ALevelSequenceActor
{
	GENERATED_BODY()

protected:

	UPROPERTY(VisibleAnywhere, Category="Spline")
	TObjectPtr<USplineComponent> Spline;

	TWeakObjectPtr<AActor> ReferenceActor;

public:	
	
	ASplineLevelSequenceActor(const FObjectInitializer& ObjectInit);

	UFUNCTION(BlueprintCallable, Category="Actor")
	void SetReferenceActor(AActor* NewReference);

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};