#include "StateMachine/General/EventMapNode.h"

void UEventMapNode::Event_Inner_Implementation(FName Event)
{
	if (auto Handle = this->EventMapInternal.Find(Event))
	{
		Handle->ExecuteIfBound();
	}
}

void UEventMapNode::EventWithData_Inner_Implementation(FName Event, UObject* Data)
{
	if (auto Handle = this->EventWithDataMapInternal.Find(Event))
	{
		Handle->ExecuteIfBound(Data);
	}
}

void UEventMapNode::Initialize_Inner_Implementation() 
{
	for (const auto& Pair : this->EventMap) 
	{
		FEventHandler f;
		f.BindUFunction(this, Pair.Value);
		this->EventMapInternal.Add(Pair.Key, f);
	}
}

TArray<FString> UEventMapNode::HandlerNameOptions() const
{
	TArray<FString> Names;
	auto Base = this->FindFunction("HandlerPrototype");

	for (TFieldIterator<UFunction> FIT(this->GetClass(), EFieldIteratorFlags::IncludeSuper); FIT; ++FIT)
	{
		UFunction* f = *FIT;

		if (f->IsSignatureCompatibleWith(Base))
		{
			if (f->GetName() != "EmitEvent" && f->GetName() != "Event_Inner")
			{
				Names.Add(f->GetName());
			}
		}	
	}

	Names.Sort([](const FString& A, const FString& B) { return A < B; });
	return Names;
}

TArray<FString> UEventMapNode::HandlerWithDataNameOptions() const
{
	TArray<FString> Names;
	auto Base = this->FindFunction("HandlerWithDataPrototype");

	for (TFieldIterator<UFunction> FIT(this->GetClass(), EFieldIteratorFlags::IncludeSuper); FIT; ++FIT)
	{
		UFunction* f = *FIT;

		if (f->IsSignatureCompatibleWith(Base))
		{
			if (f->GetName() != "EmitEvent" && f->GetName() != "Event_Inner")
			{
				Names.Add(f->GetName());
			}
		}
	}

	Names.Sort([](const FString& A, const FString& B) { return A < B; });
	return Names;
}