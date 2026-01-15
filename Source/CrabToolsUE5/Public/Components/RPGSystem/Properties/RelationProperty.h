#pragma once

#include "Components/RPGSystem/RPGProperty.h"
#include "GameplayTagContainer.h"
#include "Templates/TypeHash.h"
#include "RelationProperty.generated.h"

class UTagProperty;

USTRUCT(BlueprintType)
struct FRelationPropertyPair
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Relation")
	FGameplayTag TagOne;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Relation")
	FGameplayTag TagTwo;


	FRelationPropertyPair() {}
	FRelationPropertyPair(FGameplayTag First, FGameplayTag Second) : TagOne(First), TagTwo(Second) {}

	bool operator==(const FRelationPropertyPair& Other) const
	{
		return this->TagOne == Other.TagOne && this->TagTwo == Other.TagTwo;
	}

	friend FORCEINLINE uint32 GetTypeHash(const FRelationPropertyPair& Pair)
	{
		return HashCombineFast(GetTypeHash(Pair.TagOne), GetTypeHash(Pair.TagTwo));
	}
};


UCLASS(Blueprintable)
class CRABTOOLSUE5_API URelationProperty : public URPGProperty
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Property",
		meta = (AllowPrivateAccess))
	FGameplayTag DefaultRelation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Property",
		meta = (AllowPrivateAccess))
	TMap<FRelationPropertyPair, FGameplayTag> Relations;

public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Relations")
	FGameplayTag GetDefaultRelation() const { return this->DefaultRelation; }

	UFUNCTION(BlueprintCallable, Category = "Relations")
	void SetDefaultRelation(FGameplayTag NewDefault) { this->DefaultRelation = NewDefault; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Relations")
	FGameplayTag GetRelation(UTagProperty* TagOne, UTagProperty* TagTwo) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Relations")
	FGameplayTagContainer GetRelations(UTagProperty* Tag, UTagSetProperty* TagProp) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Relations")
	FGameplayTag GetRelationFromTags(FGameplayTag One, FGameplayTag Two) const;


	UFUNCTION(BlueprintCallable, Category = "Relations")
	void SetRelation(FGameplayTag TagOne, FGameplayTag TagTwo, FGameplayTag Relation);
};
