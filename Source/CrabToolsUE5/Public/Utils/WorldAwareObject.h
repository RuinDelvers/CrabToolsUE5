#pragma once

#include "CoreMinimal.h"
#include "WorldAwareObject.generated.h"

/**
 * Utility object that exposes GetWorld to UObjects.
 */
UCLASS(Blueprintable)
class CRABTOOLSUE5_API UWorldAwareObject : public UObject
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadOnly, meta = (ExposeOnSpawn = true, AllowPrivateAccess))
	bool bAllowOuterOwner = true;

	UPROPERTY(BlueprintReadOnly, Transient, DuplicateTransient,
		meta = (ExposeOnSpawn = true, AllowPrivateAccess))
	TObjectPtr<AActor> Owner;

public:

	virtual UWorld* GetWorld() const override;
};
