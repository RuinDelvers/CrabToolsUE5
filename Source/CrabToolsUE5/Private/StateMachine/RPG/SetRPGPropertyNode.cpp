#include "StateMachine/RPG/SetRPGPropertyNode.h"
#include "Components/RPGSystem/RPGComponent.h"
#include "Components/RPGSystem/RPGProperty.h"

void USetRPGPropertyNode::Initialize_Inner_Implementation()
{
	this->Setter->Initialize(
		CastChecked<URPGComponent>(this->GetActorOwner()->FindComponentByClass(this->RPGClass.LoadSynchronous())));
}

void USetRPGPropertyNode::Enter_Inner_Implementation()
{
	if (this->bSetOnEnterOrExit && this->Setter)
	{
		this->Setter->ApplyValue();
	}
}

void USetRPGPropertyNode::Exit_Inner_Implementation()
{
	if (!this->bSetOnEnterOrExit && this->Setter)
	{
		this->Setter->ApplyValue();
	}
}

#if WITH_EDITOR
void USetRPGPropertyNode::PostEditChangeProperty(FPropertyChangedEvent& Event)
{
	Super::PostEditChangeProperty(Event);

	if (Event.GetPropertyName() == GET_MEMBER_NAME_CHECKED(USetRPGPropertyNode, PropertyName))
	{
		this->UpdateSetterObject();
	}
}

TArray<FString> USetRPGPropertyNode::GetPropertyOptions() const
{
	TArray<FString> Names;

	if (IsValid(this->RPGClass.LoadSynchronous()))
	{
		Names.Append(this->RPGClass->GetDefaultObject<URPGComponent>()->GetRPGPropertyNames(
			URPGProperty::StaticClass(),
			false));
	}

	Names.Sort([](const FString& A, const FString& B) { return A < B; });
	return Names;
}

void USetRPGPropertyNode::UpdateSetterObject()
{
	if (this->RPGClass.LoadSynchronous())
	{
		if (auto Prop = this->RPGClass->GetDefaultObject<URPGComponent>()->FindDefaultRPGPropertyByName(this->PropertyName))
		{
			if (auto SetterClass = Prop->GetSetter())
			{
				if (!(this->Setter && this->Setter->GetClass() == SetterClass))
				{
					this->Setter = NewObject<URPGSetter>(this, SetterClass);
					this->Setter->SetExternalControl(true, this->RPGClass, this->PropertyName);
				}
			}
			else
			{
				this->Setter = nullptr;
			}
		}
	}
	else
	{
		this->PropertyName = NAME_None;
		this->Setter = nullptr;
	}

	if (this->Setter)
	{
		this->Setter->SetPropertyName(this->PropertyName);
	}
}
#endif