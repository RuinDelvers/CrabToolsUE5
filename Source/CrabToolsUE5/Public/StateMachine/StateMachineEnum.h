#pragma once

#include "CoreMinimal.h"

#include "StateMachineEnum.generated.h"

/*
 * This enum is a generic enum for StateNodes that follow a simple procedure for piped data.
 * The use case for this is used in AbilityNode and TargetingNode, and the general case is as follows:
 * 
 * - You send data to the node for it to use.
 * - The node itself could pipe the data it received, because it is a container for data.
 * - Or you could just pipe the data the node derives (e.g. a targeting controller) instead.
 * 
 * The derived case can be extended by other nodes with further enums, but these cases are the most common.
 */
UENUM(BlueprintType)
enum class EStateMachineGenericPipedDataProcedure : uint8
{
	NONE   UMETA(DisplayName = "None"),
	CACHED UMETA(DisplayName = "Cached"),
	DERIVED UMETA(DisplayName = "Derived"),
};

UENUM(BlueprintType)
enum class EStateMachineAccessibility : uint8
{
	/* All access available. */
	PUBLIC          UMETA(DisplayName = "Public"),
	/* Visible anywhere, but cannot be overwritten or extended in children. */
	VISIBLE_ANYWHERE    UMETA(DisplayName = "VisibleAnywhere"),
	/* For State Machines, Children can use them in Hierarchy Nodes. For Nodes, they can be transitioned to and from.  */
	PROTECTED       UMETA(DisplayName = "Protected"),
	/* Can only be seen/used in the machine that defines them. */
	PRIVATE         UMETA(DisplayName = "Private"),
	/* For State Machines, new nodes can be added. For Nodes, they can have their data overriden.*/
	OVERRIDEABLE    UMETA(DisplayName = "Overrideable"),

};

namespace StateMachineAccessibility
{
	CRABTOOLSUE5_API bool IsChildVisible(EStateMachineAccessibility Access);
	CRABTOOLSUE5_API bool IsPublic(EStateMachineAccessibility Access);
	CRABTOOLSUE5_API bool IsExtendible(EStateMachineAccessibility Access);
	CRABTOOLSUE5_API bool IsOverrideable(EStateMachineAccessibility Access);
}