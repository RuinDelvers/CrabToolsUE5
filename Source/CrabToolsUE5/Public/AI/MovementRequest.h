#pragma once

#include "CoreMinimal.h"
#include "MovementRequest.generated.h"

class UNavigationPath;

UENUM(BlueprintType)
enum class EAIMovementRequestType : uint8
{
	/* Empty request, useful when it isn't defined for other types of behaviour. */
	NONE        UMETA(DisplayName = "None"),
	TO_ACTOR    UMETA(DisplayName = "To Actor"),
	TO_LOCATION UMETA(DisplayName = "To Location"),
	PAUSE       UMETA(DisplayName = "Pause"),
	RESUME      UMETA(DisplayName = "Resumt"),

	STOP        UMETA(DisplayName = "Stop"),	
	/* Use a specific path to follow without computing a new one.*/
	PATH        UMETA(DisplayName = "Path"),
};


UINTERFACE(MinimalAPI, BlueprintType)
class UMovementRequestInterface : public UInterface
{
	GENERATED_BODY()
};

class IMovementRequestInterface
{
	GENERATED_BODY()

public:


	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	EAIMovementRequestType GetRequestType() const;
	virtual EAIMovementRequestType GetRequestType_Implementation() const { return EAIMovementRequestType::PAUSE; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	AActor* GetActor() const;
	virtual AActor* GetActor_Implementation() const { return nullptr; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	FVector GetLocation() const;
	virtual FVector GetLocation_Implementation() const { return FVector::ZeroVector; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	UNavigationPath* GetNavPath() const;
	virtual UNavigationPath* GetNavPath_Implementation() const { return nullptr; }
}; 

UCLASS(Blueprintable, Category = "AI")
class CRABTOOLSUE5_API UAISimpleMovementRequest : public UObject, public IMovementRequestInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="AI")
	static FVector GetEndLocation(UNavigationPath* Path);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AI")
	static FVector GetStartLocation(UNavigationPath* Path);

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI", meta=(ExposeOnSpawn))
	TObjectPtr<UNavigationPath> Path;

public:

	virtual EAIMovementRequestType GetRequestType_Implementation() const override { return this->RequestType; }
	virtual AActor* GetActor_Implementation() const override { return this->ActorDestination; }
	virtual FVector GetLocation_Implementation() const override { return this->LocationDestination; }
};