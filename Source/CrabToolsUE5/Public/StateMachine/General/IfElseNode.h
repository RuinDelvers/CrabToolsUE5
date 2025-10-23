#pragma once

#include "StateMachine/StateMachine.h"
#include "IfElseNode.generated.h"

class UGenericPropertyBinding;

UENUM()
enum class EIfElseNodeFlagType
{
	FLAG     UMETA(DisplayName="Flag"),
	PROPERTY UMETA(DisplayName = "Property"),
};

/**
 * 
 */
UCLASS(Blueprintable, Category = "StateMachine")
class CRABTOOLSUE5_API UIfElseNode : public UStateNode
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, Category="Condition",
		meta=(AllowPrivateAccess))
	EIfElseNodeFlagType ConditionType = EIfElseNodeFlagType::FLAG;

	UPROPERTY(EditAnywhere, Category = "Condition",
		meta = (AllowPrivateAccess,
			EditCondition="ConditionType==EIfElseNodeFlagType::FLAG",
			EditConditionHides))
	bool bFlag = true;

	UPROPERTY(VisibleAnywhere, Category = "Condition",
		meta = (AllowPrivateAccess,
			EditCondition = "ConditionType==EIfElseNodeFlagType::PROPERTY",
			EditConditionHides,
			ShowInnerProperties))
	TObjectPtr<UGenericPropertyBinding> Property;

	UPROPERTY(EditAnywhere, Instanced, Category="Nodes",
		meta=(AllowPrivateAccess))
	TObjectPtr<UStateNode> TrueNode;

	UPROPERTY(EditAnywhere, Instanced, Category = "Nodes",
		meta=(AllowPrivateAccess))
	TObjectPtr<UStateNode> FalseNode;

public:

	UIfElseNode();

	UStateNode* GetNode() const;
	bool GetFlag() const;

protected:

	virtual void Initialize_Inner_Implementation() override;
	virtual void Tick_Inner_Implementation(float DeltaTime) override;
	virtual void Event_Inner_Implementation(FName Event) override;
	virtual void EventWithData_Inner_Implementation(FName InEvent, UObject* Data) override;
	virtual void Enter_Inner_Implementation() override;
	virtual void EnterWithData_Inner_Implementation(UObject* Data) override;
	virtual void Exit_Inner_Implementation() override;
	virtual void ExitWithData_Inner_Implementation(UObject* Data) override;
	virtual bool RequiresTick_Implementation() const override;
	virtual void SetActive_Inner_Implementation(bool bNewActive) override;
	virtual bool HasPipedData_Implementation() const override;
	virtual UObject* GetPipedData_Implementation() override;
	

	#if WITH_EDITORONLY_DATA
		virtual void GetEmittedEvents(TSet<FName>& Events) const override;
	#endif

};
