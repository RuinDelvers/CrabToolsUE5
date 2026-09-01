#pragma once

#include "CoreMinimal.h"
#include "CurveTools.generated.h"

/*
 * Struct that represents a simple line curve segment. It is defined by two points and a duration. This struct is stateful
 * when used, but when copied its state is reset.
 */
USTRUCT(BlueprintType)
struct CRABTOOLSUE5_API FLinearVelocityCurve
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category="Curve")
	FVector Start;

	UPROPERTY(EditAnywhere, Category = "Curve")
	FVector End;

	UPROPERTY(EditAnywhere, Category = "Curve")
	float Time = 1.0f;

private:

	FVector Direction;
	float CurrentTime = 0.0f;

public:

	FLinearVelocityCurve() {}
	FLinearVelocityCurve(FVector InitStart, FVector InitEnd, float InitTime);
	FLinearVelocityCurve(const FLinearVelocityCurve& InCurve);

	FVector Step(float DeltaTime);
	bool IsFinished() const { return CurrentTime >= Time; }

	/* Initializes the derived variables for this curve. Needed for default constructor useage, or blueprint usage.*/
	void Initialize();
};

template<>
struct TStructOpsTypeTraits<FLinearVelocityCurve> : public TStructOpsTypeTraitsBase2<FLinearVelocityCurve>
{
	enum
	{
		WithCopy = true
	};
};



/*
 * A curve representing an arc. This curve is defined by
 * 
 * - Start Point
 * - End Point
 * - Gravity (Acceleration)
 * - Arc duration or speed.
 */
USTRUCT(BlueprintType)
struct CRABTOOLSUE5_API FArcPointCurve
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category="Curve")
	FVector Start;

	UPROPERTY(EditAnywhere, Category = "Curve")
	FVector End;

	UPROPERTY(EditAnywhere, Category = "Curve")
	float Gravity = -9.8f;

	/* The duration of travel along the curve*/
	UPROPERTY(EditAnywhere, Category = "Curve")
	float Time = 0.0;

private:

	FVector Velocity;
	float CurrentTime = 0.0f;

public:

	static FVector ComputeVelocityVector(const FVector& Start, const FVector& End, float Gravity, float Time);

public:

	FArcPointCurve() {}
	FArcPointCurve(FVector InitStart, FVector InitEnd, float InitGravity, float InitTime);

	FVector Step(float DeltaTime);
	bool IsFinished() const;
};

UCLASS()
class UCurveToolsHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Curve")
	static void InitLinearVelocityCurve(UPARAM(Ref) FLinearVelocityCurve& Curve);

	UFUNCTION(BlueprintCallable, Category="Curve")
	static FVector ComputeVelocityVector(const FVector& Start, const FVector& End, float Gravity, float Time);
};