#include "DataAssets/SortableGameplayTagSet.h"

void USortableGameplayTagSet::Sort()
{	
	this->TagSet.Sort(
		[&](const FGameplayTag& A, const FGameplayTag& B) {
			return A.ToString() < B.ToString();
		});

	this->Modify();
}

bool USortableGameplayTagSet::MatchesAny(const FGameplayTag& Tag) const
{
	for (const auto& ParentTag : this->TagSet)
	{
		if (ParentTag.MatchesTag(Tag))
		{
			return true;
		}
	}

	return false;
}

void USortableGameplayTagSet::Apply(const FApplyDelegate& Callback)
{
	for (const auto& Tag : this->TagSet)
	{
		Callback.Execute(Tag);
	}
}
