#pragma once

#include "Components/RPGSystem/RPGProperty.h"
#include "GameplayTagContainer.h"
#include "TagProperty.generated.h"

UCLASS(BlueprintType)
class CRABTOOLSUE5_API UTagProperty: public URPGProperty
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Property",
		meta=(AllowPrivateAccess))
	FGameplayTag Tag;

public:

	void SetTag(FGameplayTag NewTag) { this->Tag = NewTag; }
	FGameplayTag GetTag() const { return this->Tag; }
};

UCLASS(BlueprintType)
class CRABTOOLSUE5_API UTagSetProperty : public URPGProperty
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Property",
		meta = (AllowPrivateAccess))
	FGameplayTagContainer Tags;

public:

	void AddTag(FGameplayTag NewTag) { this->Tags.AddTag(NewTag); }
	void RemoveTag(FGameplayTag NewTag) { this->Tags.RemoveTag(NewTag); }
	const FGameplayTagContainer& GetTags() const { return this->Tags; }
};