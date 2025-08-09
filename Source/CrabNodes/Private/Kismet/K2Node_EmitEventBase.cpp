#include "Kismet/K2Node_EmitEventBase.h"

UK2Node_EmitEventBase::UK2Node_EmitEventBase(const FObjectInitializer& ObjectInitializer)
{

}

bool UK2Node_EmitEventBase::IsActionFilteredOut(FBlueprintActionFilter const& Filter)
{
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
}