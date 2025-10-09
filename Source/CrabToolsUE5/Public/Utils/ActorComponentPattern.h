#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ActorComponentPattern.generated.h"

class UActorComponent;

UENUM()
enum class EActorComponentPatternLayerType
{
	TAGS  UMETA(DisplayName = "By Tag"),
	CLASS UMETA(DisplayName = "By Class"),
};

USTRUCT(BlueprintType)
struct FActorComponentPatternLayer
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Layer")
	EActorComponentPatternLayerType Type = EActorComponentPatternLayerType::CLASS;

	UPROPERTY(EditAnywhere, Category = "Layer",
		meta=(EditCondition="Type==EActorComponentPatternLayerType::CLASS", EditConditionHides))
	TSubclassOf<UActorComponent> ComponentClass;

	UPROPERTY(EditAnywhere, Category = "Layer",
		meta = (EditCondition = "Type==EActorComponentPatternLayerType::TAGS", EditConditionHides))
	TArray<FName> RequiredTags;

	void FindComponents(TArray<TObjectPtr<UActorComponent>>& Comps) const;

	bool Matches(UActorComponent* Comp) const;
};

USTRUCT(BlueprintType)
struct FActorComponentPattern
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Pattern")
	TArray<FActorComponentPatternLayer> Layers;

	 void FindComponents(TArray<TObjectPtr<UActorComponent>>& Comps) const;

	 bool IsEmpty() const { return Layers.Num() == 0; }
};



UCLASS(Blueprintable, EditInlineNew, Category = "Actors|Components")
class CRABTOOLSUE5_API UActorComponentPatternRef : public UDataAsset
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, Category="Pattern")
	FActorComponentPattern Pattern;

public:

	UActorComponent* FindComponent(AActor* Actor) const;
	UActorComponent* FindComponentByClass(AActor* Actor, TSubclassOf<UActorComponent> CompClass) const;

	template <class T>
	T* FindComponentByClass(AActor* Actor)
	{
		return Cast<T>(this->FindComponentByClass(Actor, T::StaticClass()));
	}
};