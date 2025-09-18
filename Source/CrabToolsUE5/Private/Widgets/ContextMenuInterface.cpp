#include "Widgets/ContextMenuInterface.h"

void UContextMenuEntry::PerformActions()
{
	this->OnAction.Broadcast();
	this->OnActionNative.Broadcast();
}

TArray<FString> UContextMenuEntry::GetFunctionOptions() const
{
	TArray<FString> Names;

	if (auto Obj = this->GetOuter())
	{
		auto Class = Obj->GetClass();

		for (TFieldIterator<UFunction> It(Class, EFieldIterationFlags::IncludeSuper) ; It; ++It)
		{
			UFunction* f = *It;

			Names.Add(f->GetName());
		}
	}

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}