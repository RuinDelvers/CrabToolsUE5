#pragma once

#include "Components/CapsuleComponent.h"
#include "SimpleLedgeDetector.generated.h"

class UCapsuleComponent;
class UCharacterMovementComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ULedgeDetectorActivationComponent : public UCapsuleComponent
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Physics", meta=(AllowPrivateAccess))
	TEnumAsByte<ECollisionChannel> OverlapChannel = ECC_WorldStatic;

protected:

	virtual void BeginPlay() override;

private:

	ULedgeDetectorActivationComponent();

	UFUNCTION()
	void OnBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);
};

UCLASS(Blueprintable, Abstract, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class USimpleLedgeDetectorComponent : public UActorComponent
{
	GENERATED_BODY()

private:

	#if WITH_EDITORONLY_DATA
		UPROPERTY(EditAnywhere, Category="LedgeDetection", meta=(AllowPrivateAccess))
		bool bDrawDebug = false;
	#endif

	UPROPERTY(EditAnywhere, Category = "LedgeDetection", meta = (AllowPrivateAccess))
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

	UPROPERTY(EditAnywhere, Category = "LedgeDetection", meta = (AllowPrivateAccess))
	float AboveLedgeDistance = 100;

	UPROPERTY(EditAnywhere, Category = "LedgeDetection", meta = (AllowPrivateAccess))
	float BelowLedgeDistance = 100;

	UPROPERTY(EditAnywhere, Category = "LedgeDetection", meta = (AllowPrivateAccess))
	float ForwardDistance = 10;

	UPROPERTY(EditAnywhere, Category = "LedgeDetection", meta = (AllowPrivateAccess))
	bool bUseCapsuleRadiusForForward = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LedgeDetection", meta = (AllowPrivateAccess))
	FHitResult Result;

	UPROPERTY(Transient)
	TObjectPtr<UCapsuleComponent> Collision;

	UPROPERTY(Transient)
	TObjectPtr<UCharacterMovementComponent> Movement;

	UPROPERTY(VisibleAnywhere, Category = "LedgeDetection")
	TSet<TObjectPtr<UObject>> Activators;

public:

	USimpleLedgeDetectorComponent();

	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;
	virtual void OnComponentCreated() override;

	UFUNCTION(BlueprintCallable, Category="LedgeDetection")
	void SetOwnerLocationAtResult() const;

	UFUNCTION(BlueprintCallable, Category = "LedgeDetection")
	void AddActivator(UObject* Obj);

	UFUNCTION(BlueprintCallable, Category = "LedgeDetection")
	void RemoveActivator(UObject* Obj);

	UFUNCTION(BlueprintCallable, Category = "LedgeDetection")
	void PerformTrace();

protected:
	
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent, Category = "LedgeDetection|Middle")
	void AboveLedgeDetected();
	virtual void AboveLedgeDetected_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, Category="LedgeDetection|Below")
	void BelowLedgeDetected();
	virtual void BelowLedgeDetected_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, Category = "LedgeDetection|Middle")
	void MiddleLedgeDetected();
	virtual void MiddleLedgeDetected_Implementation() {}

private:

};