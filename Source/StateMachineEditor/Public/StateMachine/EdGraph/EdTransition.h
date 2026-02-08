#pragma once

#include "StateMachine/EdGraph/EdBaseNode.h"
#include "StateMachine/IStateMachineLike.h"
#include "EdTransition.generated.h"

class UTransitionCondition;
class UTransitionDataCondition;
class UEdBaseNode;
class UEdBaseStateNode;
class UEdStateGraph;

USTRUCT(BlueprintType)
struct FEventTransitionData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "StateMachineEditor")
	TObjectPtr<UTransitionCondition> Condition;

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "StateMachineEditor")
	TObjectPtr<UTransitionDataCondition> DataCondition;
};

USTRUCT(BlueprintType)
struct FEdTransitionDocumentation
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Documentation")
	FText Comment;

};

UCLASS(MinimalAPI)
class UEdTransition : public UEdBaseNode, public IStateTransitionLike
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category="StateMachineEditor",
		meta=(GetKeyOptions="GetEventOptions"))
	TMap<FName, FEventTransitionData> EventToConditionMap;

public:

	UEdTransition();

	UPROPERTY()
	class UEdGraph* Graph;

	void AppendUsedEvents(TSet<FName>& InEvents) const;

	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual bool Modify(bool bAlwaysMarkDirty = true) override;
	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;
	virtual void PrepareForCopying() override;

	virtual UEdGraphPin* GetInputPin() const { return Pins[0]; }
	virtual UEdGraphPin* GetOutputPin() const { return Pins[1]; }

	virtual bool CanDelete() const override { return true; }
	virtual bool CanCopy() const { return true; }
	virtual bool CanCut() const { return true; }

	void CreateConnections(UEdBaseStateNode* Start, UEdBaseStateNode* End);
	virtual void Delete() override;

	UEdBaseStateNode* GetStartNode() const;
	UEdBaseStateNode* GetEndNode() const;
	TMap<FName, FTransitionDataSet> GetTransitionData(FNodeVerificationContext& Context);

	virtual TArray<FString> GetSourceEventOptions() const override { return this->GetEventOptions(); }

	UFUNCTION()
	TArray<FString> GetEventOptions() const;
};