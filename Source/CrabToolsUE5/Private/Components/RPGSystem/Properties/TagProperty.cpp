#include "Components/RPGSystem/Properties/TagProperty.h"

void UTagProperty::SetTag(FGameplayTag NewTag)
{
	this->Tag = NewTag;
	this->OnPropertyChanged.Broadcast(this);
}

void UTagSetProperty::AddTag(FGameplayTag NewTag)
{
	this->Tags.AddTag(NewTag);
	this->OnPropertyChanged.Broadcast(this);
}

void UTagSetProperty::AppendTags(const FGameplayTagContainer& Cont)
{
	this->Tags.AppendTags(Cont);
	this->OnPropertyChanged.Broadcast(this);
}

void UTagSetProperty::RemoveTag(FGameplayTag NewTag)
{
	this->Tags.RemoveTag(NewTag);
	this->OnPropertyChanged.Broadcast(this);
}
