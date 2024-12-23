#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"
#include "EdGraph/EdGraphNode.h"
#include "SGraphPin.h"
#include "StateMachine/StateMachine.h"
#include "EdBaseNode.generated.h"

class UEdStateGraph;

/* Base Graph Node for both states and edges. */
UCLASS(MinimalAPI)
class UEdBaseNode : public UEdGraphNode
{
	GENERATED_BODY()

public:

	/* Events for changes to this node. */
	class FEditorEvents
	{
	public:
		DECLARE_MULTICAST_DELEGATE_TwoParams(FNameChanged, FName, FName)
		FNameChanged OnNameChanged;

		DECLARE_MULTICAST_DELEGATE(FNodeDeleted)
		FNodeDeleted OnNodeDeleted;

		DECLARE_MULTICAST_DELEGATE_OneParam(FNodeError, FText)
		FNodeError OnNodeError;
	} 
	Events;

	virtual void ClearEvents();
	UEdStateGraph* GetStateGraph() const;

	void Inspect();
};

/* Base State node to be used for states in the State Machine Graph. */
UCLASS(Abstract, MinimalAPI)
class UEdBaseStateNode : public UEdBaseNode, public IStateLike
{
	GENERATED_BODY()

public:

	UEdBaseStateNode() {}
	virtual ~UEdBaseStateNode() {}


	virtual void AutowireNewNode(UEdGraphPin* FromPin) override;
	virtual void AllocateDefaultPins() override;
	virtual UEdGraphPin* GetInputPin() const { return Pins[0]; }
	virtual UEdGraphPin* GetOutputPin() const { return Pins[1]; }
	/* Returns the name of this state when compiled. */
	virtual FName GetStateName() const { return NAME_None; }
	virtual FName SetStateName(FName NewName) { return NAME_None; }
	/* Returns the name displayed for graph nodes and UI purposes. */
	virtual FName GetNodeName() const { return NAME_None; }
	virtual bool HasEvent(FName EName) { return false; }
	virtual void RenameNode(FName Name) {};

	/* Begin IStateLike Interface */
	virtual TArray<FString> GetEnterStates() const override;
	virtual TArray<FString> GetExitStates() const override;
	/* End IStateLike Interface */
};