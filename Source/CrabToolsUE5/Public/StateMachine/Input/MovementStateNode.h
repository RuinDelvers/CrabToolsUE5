#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "StateMachine/StateMachine.h"
#include "Components/PerspectiveManager.h"
#include "GameFramework/Pawn.h"
#include "MovementStateNode.generated.h"

/**
 *
 */
UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced, Category = "StateMachine|Input")
class CRABTOOLSUE5_API UMovementStateNode : public UStateNode
{
	GENERATED_BODY()

	// Possible perspective of the owning pawn.
	TWeakObjectPtr<UPerspectiveManager> Perspective;
	// Saved reference to the owner if it is a Pawn.
	TWeakObjectPtr<APawn>PawnOwner;

	FVector2D InputAxis;

public:

	virtual void Initialize_Inner_Implementation() override;
	virtual void Tick_Inner_Implementation(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "ProcStateMachine")
	void UpdateInputAxis(FVector2D FVec);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ProcStateMachine")
	void ApplyMovement();
	virtual void ApplyMovement_Implementation();
};