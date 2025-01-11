#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PatrolPath.generated.h"

class APatrolPath;

UCLASS(Abstract, Blueprintable, EditInlineNew, Category="Navigation")
class CRABTOOLSUE5_API UPathSequence : public UObject
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, Category = "PatrolPath")
	TObjectPtr<APatrolPath> Path;

public:

	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category="Navigation")
	int Target() const;
	virtual int Target_Implementation() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Navigation")
	int Step();
	virtual int Step_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Navigation")
	void Reset();
	virtual void Reset_Implementation() {}

	UFUNCTION(BlueprintCallable, Category="Navigation")
	FVector Point(int Index) const;

	UFUNCTION(BlueprintCallable, Category = "Navigation")
	FVector CurrentPoint() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Navigation")
	APatrolPath* GetPath() const { return this->Path; }
};

UCLASS(Blueprintable, EditInlineNew, Category = "Navigation")
class CRABTOOLSUE5_API UCyclicPathSequence : public UPathSequence
{
	GENERATED_BODY()

	/* The initial index in the patrol path to get. */
	UPROPERTY(EditAnywhere, Category = "PatrolPath", meta = (AllowPrivateAccess))
	int StartIndex = 0;
	int CurrentIndex = 0;

	/* Whether or not we are following the path forward.*/
	UPROPERTY(EditAnywhere, Category = "PatrolPath", meta = (AllowPrivateAccess))
	bool bDirection = true;


public:
	virtual int Target_Implementation() const override;
	virtual int Step_Implementation() override;
	virtual void Reset_Implementation() override { this->CurrentIndex = StartIndex; }

	int GetIndex() const { return this->CurrentIndex; }
	void SetDirection(int Index);
	void SetDirection(bool NewDirection) { this->bDirection = NewDirection; }
	void Reset();
};

USTRUCT(BlueprintType)
struct FPatrolPathState
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, Instanced, Category = "PatrolPath")
	TObjectPtr<UPathSequence> Sequence;

public:

	int Target() const;
	int Step();
	void Reset();
	int Points() const;
	FVector Point() const;

	/* Return whether or not this state can be valid to traverse. Requires a path and at least two points. */
	operator bool() const;
};


UCLASS()
class UPatrolPathLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="PatrolPath")
	static FVector GetTarget(const FPatrolPathState& State);
};

USTRUCT(BlueprintType)
struct FPatrolPathData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AI,
		meta = (MakeEditWidget))
	FVector Point;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AI,
		meta = (AllowPrivateAccess))
	TObjectPtr<UObject> Data;
};

UCLASS(Blueprintable, Category = "Navigation")
class CRABTOOLSUE5_API APatrolPath : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AI,
		meta = (AllowPrivateAccess))
	bool bIsCycle = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AI,
		meta = (MakeEditWidget, AllowPrivateAccess, ShowOnlyInnerProperties))
	TArray<FPatrolPathData> Data;
	

	#if WITH_EDITORONLY_DATA
		UPROPERTY()
		TObjectPtr<class UBillboardComponent> EditorSprite;

		UPROPERTY()
		TObjectPtr<class USplineComponent> PathSpline;
	#endif

public:	

	APatrolPath();	

	UFUNCTION(BlueprintCallable, Category = "AI")
	virtual FVector FindClosestPoint(AActor* Patroller);

	UFUNCTION(BlueprintCallable, Category = "AI")
	virtual int FindClosestIndex(AActor* Patroller);

	/* Return the patrol point in World Space coordinates. */
	UFUNCTION(BlueprintPure, Category = "AI")
	FVector Get(int i);

	UFUNCTION(BlueprintPure, Category = "AI")
	FORCEINLINE int Num() const { return this->Data.Num(); }
	FORCEINLINE bool IsCycle() const { return this->bIsCycle; }
	void SetIsCycle(bool bNewCycleState) { this->bIsCycle = bNewCycleState; }

	#if WITH_EDITOR
		virtual void PreSave(FObjectPreSaveContext SaveContext) override;
		virtual void PostSaveRoot(FObjectPostSaveRootContext SaveContext) override;
		virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
		void ToggleDisplay();
	#endif

private:

	#if WITH_EDITOR
		void InitArrows();
	#endif
};
