#include "StateMachine/Widgets/Utils.h"
#include "EditorCategoryUtils.h"
#include "StateMachine/StateMachine.h"
#include "StateMachine/EdGraph/EdStateNode.h"

bool StateMachineEditor::Widgets::DetailWidgetPropertyFilter(const FPropertyAndParent& PropertyAndParent)
{
	auto Properties = PropertyAndParent.Property.GetMetaDataMap();

	// If private, do not show at all.
	if (PropertyAndParent.Property.GetBoolMetaData("BlueprintPrivate"))
	{
		return false;
	}

	if (auto Class = PropertyAndParent.Property.GetOwnerClass())
	{
		if (PropertyAndParent.Property.GetOwnerClass()->IsChildOf<UStateNode>())
		{
			// Hide emitted events for this viewer, as editing it would be incorrect.
			if (PropertyAndParent.Property.GetFName() == "EmittedEvents")
			{
				return false;
			}

			// State nodes will use hide categories to control view of properties they use, but don't want editable.
			if (Properties)
			{
				if (auto Category = Properties->Find("Category"))
				{
					TArray<FString> ClassHideCategories;
					FEditorCategoryUtils::GetClassHideCategories(Class, ClassHideCategories);

					if (ClassHideCategories.Contains(*Category))
					{
						return false;
					}
				}
			}
		}
		else if (PropertyAndParent.Property.GetOwnerClass()->IsChildOf<UEdStateNode>())
		{
			// Hide nodes, as they are uniquely managed.
			if (PropertyAndParent.Property.GetFName() == "Nodes")
			{
				return false;
			}
		}
	}

	return true;
}