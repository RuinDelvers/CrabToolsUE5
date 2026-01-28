#include "Components/RPGSystem/Properties/RelationProperty.h"
#include "Components/RPGSystem/Properties/TagProperty.h"

FGameplayTag UTernaryRelationProperty::GetRelation(UTagProperty* TagOne, UTagProperty* TagTwo) const
{
	return this->GetRelationFromTags(TagOne->GetTag(), TagTwo->GetTag());
}

FGameplayTagContainer UTernaryRelationProperty::GetRelations(UTagProperty* Tag, UTagSetProperty* TagProp) const
{
	auto ComputedRelations = FGameplayTagContainer();
	auto SelfTag = Tag->GetTag();

	for (auto& Tags : TagProp->GetTags())
	{
		ComputedRelations.AddTag(this->GetRelationFromTags(SelfTag, Tags));
	}

	return ComputedRelations;
}


FGameplayTag UTernaryRelationProperty::GetRelationFromTags(FGameplayTag One, FGameplayTag Two) const
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

void UTernaryRelationProperty::SetRelation(FGameplayTag TagOne, FGameplayTag TagTwo, FGameplayTag Relation)
{
	auto Pair = FRelationPropertyPair(TagOne, TagTwo);

	this->Relations.Add(Pair, Relation);
}

FGameplayTag UBinaryRelationProperty::GetRelation(UTagProperty* Tag) const
{
	if (auto Relation = this->Relations.Find(Tag->GetTag()))
	{
		return *Relation;
	}
	else
	{
		return this->DefaultRelation;
	}
}

FGameplayTagContainer UBinaryRelationProperty::GetRelations(UTagSetProperty* TagProp) const
{
	auto ComputedRelations = FGameplayTagContainer();

	for (auto& Tags : TagProp->GetTags())
	{
		ComputedRelations.AddTag(this->GetRelationFromTags(Tags));
	}

	return ComputedRelations;
}

FGameplayTag UBinaryRelationProperty::GetRelationFromTags(FGameplayTag Tag) const
{
	if (auto Relation = this->Relations.Find(Tag))
	{
		return *Relation;
	}
	else
	{
		return this->DefaultRelation;
	}
}

void UBinaryRelationProperty::AppendRelations(const TMap<FGameplayTag, FGameplayTag>& NewRelations)
{
	this->Relations.Append(NewRelations);
	this->OnPropertyChanged.Broadcast(this);
}

void UBinaryRelationProperty::SetRelation(FGameplayTag Tag, FGameplayTag Relation)
{
	this->Relations.Add(Tag, Relation);
	this->OnPropertyChanged.Broadcast(this);
}
