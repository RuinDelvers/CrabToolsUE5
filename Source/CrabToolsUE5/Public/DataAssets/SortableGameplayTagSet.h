#pragma 

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "SortableGameplayTagSet.generated.h"

/* Simple set of gameplay tags that can be sorted alphabetically. */
UCLASS(BlueprintType)
class USortableGameplayTagSet : public UDataAsset
{
	GENERATED_BODY()

public:

	DECLARE_DYNAMIC_DELEGATE_OneParam(FApplyDelegate, FGameplayTag, Tag);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="TagSet")
	TSet<FGameplayTag> TagSet;

public:

	UFUNCTION(CallInEditor, Category="Sorting")
	void Sort();

	UFUNCTION(BlueprintCallable, Category = "Sorting")
	bool MatchesAny(const FGameplayTag& Tag) const;

	UFUNCTION(BlueprintCallable, Category = "Sorting")
	bool Contains(const FGameplayTag& Tag) const { return this->TagSet.Contains(Tag); }

	UFUNCTION(BlueprintCallable, Category="Sorting")
	void Apply(const FApplyDelegate& Callback);
};