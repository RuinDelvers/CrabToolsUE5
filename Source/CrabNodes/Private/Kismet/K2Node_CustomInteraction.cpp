#include "Kismet/K2Node_CustomInteraction.h"

#include "Engine/Blueprint.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintEventNodeSpawner.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "K2Node_CustomEvent.h"


void UK2Node_CustomInteraction::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	// Does not work currently, as the registrar pulls an ensure that the generating class (this) is equal to
	// whatever the spawner is attempting to spawn.
	/*
	UClass* ActionKey = this->GetClass();

	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintEventNodeSpawner::Create(UK2Node_CustomEvent::StaticClass(), FName());
		NodeSpawner->DefaultMenuSignature.MenuName = FText::FromString("Add Custom Interaction...");
		check(NodeSpawner != nullptr);

		auto SetupCustomEventNodeLambda = [](UEdGraphNode* NewNode, bool bIsTemplateNode)
			{
				UK2Node_CustomEvent* EventNode = CastChecked<UK2Node_CustomEvent>(NewNode);
				EventNode->SetDelegateSignature(
					UK2Node_CustomInteraction::StaticClass()->FindFunctionByName(
						GET_FUNCTION_NAME_CHECKED(UK2Node_CustomInteraction, SignatureFunction)));
				UBlueprint* Blueprint = EventNode->GetBlueprint();

				if (!bIsTemplateNode)
				{
					EventNode->CustomFunctionName = FBlueprintEditorUtils::FindUniqueCustomEventName(Blueprint);
				}

				// Not being editable still allows renaming, just not changes of parameters.
				EventNode->bIsEditable = false;
			};

		NodeSpawner->CustomizeNodeDelegate = UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateStatic(SetupCustomEventNodeLambda);
		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
	*/
}