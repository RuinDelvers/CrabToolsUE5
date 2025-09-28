#include "Kismet/K2Node_EmitEventBase.h"
#include "Kismet2/CompilerResultsLog.h"

#define LOCTEXT_NAMESPACE "K2Node_EmitEventBase"

UK2Node_EmitEventBase::UK2Node_EmitEventBase(const FObjectInitializer& ObjectInitializer)
{

}

bool UK2Node_EmitEventBase::IsActionFilteredOut(FBlueprintActionFilter const& Filter)
{
	/*
	bool bAllBlueprintSupported = true;

	for (const auto& BP : Filter.Context.Blueprints)
	{
		if (BP)
		{
			if (BP->ParentClass && !BP->ParentClass->IsChildOf<AActor>())
			{
				bAllBlueprintSupported = false;
				break;
			}
		}
	}

	return !bAllBlueprintSupported;
	*/

	return false;
}

bool UK2Node_EmitEventBase::CheckValidEvent(class FCompilerResultsLog& CompilerContext) const
{
	bool Valid = false;

	if (auto Pin = this->GetEventPin())
	{
		FName EName = FName(Pin->DefaultValue);

		if (EName.IsNone())
		{
			this->PrintInvalidEventError(CompilerContext, EName);
		}
		else 
		{
			if (this->GetEventSet().Contains(EName))
			{
				Valid = true;
			}
			else
			{
				this->PrintInvalidEventError(CompilerContext, EName);
			}
		}
	}

	return Valid;
}

void UK2Node_EmitEventBase::PrintInvalidEventError(class FCompilerResultsLog& CompilerContext, FName EName) const
{
	FText Error = FText::FormatNamed(
		LOCTEXT("NoEventError_Error", "@@ has invalid event {EName}."),
		TEXT("EName"), FText::FromName(EName));

	CompilerContext.Error(
		*Error.ToString(),
		this);
}

#undef LOCTEXT_NAMESPACE