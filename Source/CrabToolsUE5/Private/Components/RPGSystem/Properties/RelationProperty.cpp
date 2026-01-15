#include "Components/RPGSystem/Properties/RelationProperty.h"
#include "Components/RPGSystem/Properties/TagProperty.h"

FGameplayTag URelationProperty::GetRelation(UTagProperty* TagOne, UTagProperty* TagTwo) const
{
	return this->GetRelationFromTags(TagOne->GetTag(), TagTwo->GetTag());
}

FGameplayTagContainer URelationProperty::GetRelations(UTagProperty* Tag, UTagSetProperty* TagProp) const
{
	auto ComputedRelations = FGameplayTagContainer();
	auto SelfTag = Tag->GetTag();

	for (auto& Tags : TagProp->GetTags())
	{
		ComputedRelations.AddTag(this->GetRelationFromTags(SelfTag, Tags));
	}

	return ComputedRelations;
}


FGameplayTag URelationProperty::GetRelationFromTags(FGameplayTag One, FGameplayTag Two) const
{
	auto Pair = FRelationPropertyPair(One, Two);

	if (auto Value = this->Relations.Find(Pair))
	{
		return *Value;
	}
	else
	{
		return this->DefaultRelation;
	}
}

void URelationProperty::SetRelation(FGameplayTag TagOne, FGameplayTag TagTwo, FGameplayTag Relation)
{
	auto Pair = FRelationPropertyPair(TagOne, TagTwo);

	this->Relations.Add(Pair, Relation);
}
