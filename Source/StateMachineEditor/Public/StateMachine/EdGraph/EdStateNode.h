#pragma once

#include "CoreMinimal.h"
#include "StateMachine/IStateMachineLike.h"
#include "SGraphNode.h"
#include "EdGraph/EdGraphNode.h"
#include "SGraphPin.h"
#include "StateMachine/StateMachine.h"
#include "StateMachine/EdGraph/EdBaseNode.h"
#include "EdStateNode.generated.h"

struct FStateArchetypeData;

UENUM(BlueprintType)
enum class EStateNodeType : uint8
{
	/* Will add nodes to the parent state when instantiated. */
	EXTENDED_NODE    UMETA(DisplayName = "Extends"),
	/* Will override the nodes in the parent state when instantiated. Can also override State Data. */
	OVERRIDE_NODE    UMETA(DisplayName = "Override"),
	/* Will add nodes to the parent state when instantiated. Can also override State Data. */
	OVERRIDE_EXTENDED_NODE    UMETA(DisplayName = "OverrideExtend"),
	/* Base definition of a state in an SMGraph. */
	INLINE_NODE      UMETA(DisplayName = "Inline"),
};

USTRUCT(BlueprintType)
struct FStateNodeOverrideContainer
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Instanced, Category = "Override",
		meta = (ShowInnerProperties, NoResetToDefault, EditConditionHides))
	TObjectPtr<UState> Value;

	/* This object is used to compare overriden variables when Linking changes. */
	UPROPERTY()
	TObjectPtr<UState> DefaultObject;
};

UCLASS(MinimalAPI)
class UEdStateNode : public UEdBaseStateNode
{
	GENERATED_BODY()

	/* The the current state of the select state machine debug object in the editor. */
	UPROPERTY(VisibleAnywhere, Instanced, Category="Debug", meta=(ShowInnerProperties))
	TObjectPtr<UState> DebugStateObject;

	UPROPERTY(EditDefaultsOnly, Category="StateMachine")
	EStateNodeType NodeType = EStateNodeType::INLINE_NODE;

	/* Whether or not a blueprint variable should be generated for this state. */
	UPROPERTY(EditDefaultsOnly, Category="StateMachine",
		meta = (EditCondition = "NodeType == EStateNodeType::INLINE_NODE", EditConditionHides))
	bool bIsVariable = false;

	UPROPERTY(EditDefaultsOnly, Category="StateMachine",
		meta = (EditCondition = "NodeType == EStateNodeType::INLINE_NODE", EditConditionHides))
	EStateMachineAccessibility Accessibility = EStateMachineAccessibility::PRIVATE;

	UPROPERTY(VisibleAnywhere, Category = "StateMachine")
	FName StateName;

	UPROPERTY(EditDefaultsOnly, Category = "StateMachine",
		meta = (EditCondition = "NodeType == EStateNodeType::INLINE_NODE", EditConditionHides))
	FName StateCategory;

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Nodes")
	TArray<TObjectPtr<UStateNode>> Nodes;

	UPROPERTY(VisibleAnywhere, Category="Events")
	TSet<FName> NodeEmittedEvents;

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "StateMachine",
		meta=(EditCondition="NodeType == EStateNodeType::INLINE_NODE", EditConditionHides))
	TObjectPtr<UState> StateClass;

	UPROPERTY(EditAnywhere, Category = "Override",
		meta = (EditCondition = "NodeType != EStateNodeType::INLINE_NODE", EditConditionHides,
			GetOptions="GetInheritableStates"))
	FName StateExtension;

	UPROPERTY(EditAnywhere, Category = "Override",
		meta = (
			EditCondition = "(NodeType != EStateNodeType::INLINE_NODE) && (NodeType != EStateNodeType::EXTENDED_NODE)",
			EditConditionHides,
			ShowOnlyInnerProperties))
	 FStateNodeOverrideContainer StateClassOverride;

public:

	UEdStateNode();
	virtual ~UEdStateNode();
	
	void SetNodeTemplate(UStateNode* NewNode) { this->Nodes.Add(NewNode); }
	
	FORCEINLINE void SetNodeType(EStateNodeType NewType) { this->NodeType = NewType; }
	FORCEINLINE EStateNodeType GetNodeType() const { return this->NodeType; }
	FName GetStateCategory() const { return this->StateCategory; }
	FName SetStateName(FName NewName);
	const TArray<TObjectPtr<UStateNode>>& GetStateList() const { return this->Nodes; }
	FStateArchetypeData CompileState(FNodeVerificationContext& Context, UObject* Outer);
	void Delete();
	virtual void RenameNode(FName Name);
	void UpdateStateArchetype(TSubclassOf<UState> StateClass);
	void SetDebugObject(UState* State);
	UState* GetDebugObject() const { return this->DebugStateObject; }

	virtual FName GetStateName() const override;
	/* Returns the name which should appear on graph nodes. */
	virtual FName GetNodeName() const override { return this->StateName; }
	virtual bool HasEvent(FName EName) override;
	virtual bool Modify(bool bAlwaysMarkDirty = true) override;

	/* Begin IStateLike Interface */
	virtual TArray<FString> GetEventOptions() const override;
	/* End IStateLike Interface */

	#if WITH_EDITOR
		virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
		virtual void PostLinkerChange() override;
		virtual TArray<FString> GetPropertiesOptions(const FSMPropertySearch& SearchParam) const override;
	#endif

	virtual bool IsActive() const;
	TSet<FName> GetNotifies() const;

private:

	void UpdateEmittedEvents();
	bool UpdateStateArchetypeOverride();

	UFUNCTION()
	TArray<FString> GetInheritableStates() const;

	FStateArchetypeData GetBaseCompilationData(UObject* Outer);
};