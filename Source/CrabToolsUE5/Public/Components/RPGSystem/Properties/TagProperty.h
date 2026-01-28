#pragma once

#include "Components/RPGSystem/RPGProperty.h"
#include "GameplayTagContainer.h"
#include "TagProperty.generated.h"

UCLASS(BlueprintType, CollapseCategories)
class CRABTOOLSUE5_API UTagProperty: public URPGProperty
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Relations",
		meta=(AllowPrivateAccess))
	FGameplayTag Tag;

public:

	UFUNCTION(BlueprintCallable, Category = "Property")
	void SetTag(FGameplayTag NewTag);

	UFUNCTION(BlueprintCallable, Category = "Property")
	FGameplayTag GetTag() const { return this->Tag; }
};

UCLASS(BlueprintType, CollapseCategories)
class CRABTOOLSUE5_API UTagSetProperty : public URPGProperty
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Relations",
		meta = (AllowPrivateAccess))
	FGameplayTagContainer Tags;

public:

	UFUNCTION(BlueprintCallable, Category="Property")
	void AddTag(FGameplayTag NewTag);

	UFUNCTION(BlueprintCallable, Category = "Property")
	void AppendTags(const FGameplayTagContainer& Cont);

	UFUNCTION(BlueprintCallable, Category = "Property")
	void RemoveTag(FGameplayTag NewTag);

	UFUNCTION(BlueprintCallable, Category = "Property")
	const FGameplayTagContainer& GetTags() const { return this->Tags; }
};