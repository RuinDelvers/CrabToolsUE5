#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "StateMachine/Input/InputNode.h"
#include "Components/PerspectiveManager.h"
#include "GameFramework/Pawn.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MovementInputNode.generated.h"

/**
 * Simple input node that handles standard movement for 3rd person perspectives.
 */
UCLASS(Blueprintable, EditInlineNew, Category = "StateMachine|Input")
class CRABTOOLSUE5_API UMovementInputNode : public UInputNode
{
	GENERATED_BODY()

	// Possible perspective of the owning pawn.
	TWeakObjectPtr<UPerspectiveManager> Perspective;

public:

	UMovementInputNode();

	virtual void Initialize_Inner_Implementation() override;
	virtual void TriggerCallback_Implementation(const FInputActionValue& Value);
	

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StateMachine")
	void ApplyMovement(FVector2D InputAxis);
	virtual void ApplyMovement_Implementation(FVector2D InputAxis);

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	UPerspectiveManager* GetPerspective() { return this->Perspective.Get(); }
};