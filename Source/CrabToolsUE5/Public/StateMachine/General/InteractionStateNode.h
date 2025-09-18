#pragma once

#include "StateMachine/StateMachine.h"
#include "Components/Interaction/InteractableComponent.h"
#include "InteractionStateNode.generated.h"

/**
 * Node that toggles the state for interactions on a specific actor it lives in.
 */
UCLASS(Blueprintable, Category = "StateMachine")
class CRABTOOLSUE5_API UInteractionStateNode : public UStateNode
{
	GENERATED_BODY()

	#if WITH_EDITORONLY_DATA
		UPROPERTY(EditAnywhere, Category = "StateMachine", meta = (AllowPrivateAccess))
		TSoftClassPtr<UObject> SearchActor = AActor::StaticClass();
	#endif

	UPROPERTY(EditAnywhere, Category = "StateMachine", meta = (AllowPrivateAccess,
		GetKeyOptions="GetInteractionNames"))
	TMap<FName, FInteractableStateData> EnterStateMap;

	UPROPERTY(EditAnywhere, Category = "StateMachine", meta = (AllowPrivateAccess,
		GetKeyOptions = "GetInteractionNames"))
	TMap<FName, FInteractableStateData> ExitStateMap;

	UPROPERTY(Transient)
	TObjectPtr<UInteractableComponent> Component;

public:

	virtual void Initialize_Inner_Implementation() override;
	virtual void Enter_Inner_Implementation() override;
	virtual void Exit_Inner_Implementation() override;	

private:

	#if WITH_EDITOR
		UFUNCTION()
		TArray<FString> GetInteractionNames() const;

		virtual void PostEditChangeProperty(FPropertyChangedEvent& Event) override;
	#endif	
};
