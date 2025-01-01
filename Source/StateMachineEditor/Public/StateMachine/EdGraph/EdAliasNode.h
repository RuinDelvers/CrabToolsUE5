#pragma once

#include "CoreMinimal.h"
#include "StateMachine/IStateMachineLike.h"
#include "SGraphNode.h"
#include "EdGraph/EdGraphNode.h"
#include "SGraphPin.h"
#include "StateMachine/EdGraph/EdStateNode.h"
#include "EdAliasNode.generated.h"


UCLASS(MinimalAPI)
class UEdAliasNode : public UEdBaseStateNode
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category="Alias")
	FName AliasLabel;

	UPROPERTY(EditDefaultsOnly, Category="Alias", meta=(GetOptions="GetAvailableStates"))
	TSet<FName> AliasedStates;

	/* 
	 * Whether or not the alias state set is referring to what states to alias, or what not to alias.
	 * If false, then this alis represents those states in the alias set. If true, represents any
	 * state not in the alias set.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Alias")
	bool bComplement = false;

public:

	UEdAliasNode();
	virtual ~UEdAliasNode();
	
	FName SetStateName(FName NewName) override;
	void Delete();
	bool Matches(UEdStateNode* Node) const;

	virtual FName GetStateName() const override;
	/* Returns the name which should appear on graph nodes. */
	virtual bool HasEvent(FName EName) override;
	virtual bool Modify(bool bAlwaysMarkDirty = true) override;
	virtual void RenameNode(FName Name) override;
	virtual FName GetNodeName() const override { return this->AliasLabel; }

	#if WITH_EDITOR
		virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	#endif

	const TSet<FName>& GetAliases() const { return this->AliasedStates; }
	bool IsComplement() const { return this->bComplement; }
	TSet<FName> GetAliasedStates() const;

private:

	UFUNCTION()
	TArray<FString> GetAvailableStates() const;
};