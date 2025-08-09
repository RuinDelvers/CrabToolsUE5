#include "StateMachine/General/CallFunctionNode.h"

void UCallFunctionNode::Initialize_Inner_Implementation()
{
	for (const auto& Source : this->Functions)
	{
		for (auto& Delegate : Source->GetBinding())
		{
			this->Callback.Add(Delegate);
		}		
	}
}

void UCallFunctionNode::HandleBinding(UObject* Obj, const TSet<FName>& FnNames)
{
	if (IsValid(Obj))
	{
		for (const auto& FnName : FnNames)
		{
			FScriptDelegate FnCall;
			FnCall.BindUFunction(Obj, FnName);

			if (FnCall.IsBound())
			{
				this->Callback.Add(FnCall);
			}
		}
	}
}

void UCallFunctionNode::Enter_Inner_Implementation()
{
	if (this->bCallOnEnter)
	{
		this->Callback.ProcessMulticastDelegate<UObject>(nullptr);
	}
}

void UCallFunctionNode::Exit_Inner_Implementation()
{
	if (this->bCallOnExit)
	{
		this->Callback.ProcessMulticastDelegate<UObject>(nullptr);
	}
}

TArray<FScriptDelegate> UFunctionSource::GetBinding() const
{
	TArray<FScriptDelegate> Delegates;

	for (const auto& FnName : Functions)
	{
		UObject* Obj = this->GetSource(CastChecked<UCallFunctionNode>(this->GetOuter()));

		if (this->MatchesObject(Obj))
		{
			FScriptDelegate Delegate;
			Delegate.BindUFunction(Obj, FnName);

			if (Delegate.IsBound())
			{
				Delegates.Add(Delegate);
			}
		}
	}

	return Delegates;
}

#if WITH_EDITOR

TArray<FString> UFunctionSource::GetFunctionNames() const
{
	return this->GetClassFunctions(this->GetFilterClass());
}

TArray<FString> UFunctionSource::GetClassFunctions(UClass* Class) const
{
	TArray<FString> Names;

	for (TFieldIterator<UFunction> FnIt(Class); FnIt; ++FnIt)
	{
		if (FnIt->IsSignatureCompatibleWith(this->FindFunction(GET_FUNCTION_NAME_CHECKED(UFunctionSource, SignatureFunction))))
		{
			Names.Add(FnIt->GetName());
		}
	}

	Names.Sort([](const FString& A, const FString& B) { return A < B; });

	return Names;
}
#endif