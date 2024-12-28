#pragma once

#include "CoreMinimal.h"
#include "ITargeter.generated.h"

UINTERFACE(MinimalAPI, Blueprintable, BlueprintType)
class UTargeterInterface : public UInterface
{
	GENERATED_BODY()
};

class ITargeterInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Targeting|Trace")
	FVector GetEndPoint() const;
	virtual FVector GetEndPoint_Implementation() const { return FVector::Zero(); }

	/* Returns an actor that is currently being targeted by the targeter or traced. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Targeting|Trace")
	AActor* GetTracedActor() const;
	virtual AActor* GetTracedActor_Implementation() const { return nullptr; }
};