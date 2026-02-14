#pragma once

#include "CoreMinimal.h"
#include "MovementRequest.generated.h"

UENUM()
enum class EAIMovementRequestType : uint8
{
	NONE        UMETA(DisplayName = "None"),
	TO_ACTOR    UMETA(DisplayName = "To Actor"),
	TO_LOCATION UMETA(DisplayName = "To Location"),
	PAUSE       UMETA(DisplayName = "Pause"),
	RESUME      UMETA(DisplayName = "Resumt"),
};


UINTERFACE(MinimalAPI)
class UMovementRequestInterface : public UInterface
{
	GENERATED_BODY()
};

class IMovementRequestInterface
{
	GENERATED_BODY()

public:


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	EAIMovementRequestType GetRequestType() const;
	virtual EAIMovementRequestType GetRequestType_Implementation() const { return EAIMovementRequestType::PAUSE; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	AActor* GetActor() const;
	virtual AActor* GetActor_Implementation() const { return nullptr; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	FVector GetLocation() const;
	virtual FVector GetLocation_Implementation() const { return FVector::ZeroVector; }
};

UCLASS(Blueprintable, Category = "StateMachine|AI")
class CRABTOOLSUE5_API UAISimpleMovementRequest : public UObject, public IMovementRequestInterface
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI",
		meta = (ExposeOnSpawn))
	EAIMovementRequestType RequestType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI",
		meta = (ExposeOnSpawn))
	TObjectPtr<AActor> ActorDestination;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI",
		meta = (ExposeOnSpawn))
	FVector LocationDestination;

public:

	virtual EAIMovementRequestType GetRequestType_Implementation() const override { return this->RequestType; }
	virtual AActor* GetActor_Implementation() const override { return this->ActorDestination; }
	virtual FVector GetLocation_Implementation() const override { return this->LocationDestination; }
};