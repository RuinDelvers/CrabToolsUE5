#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "StateMachine/Input/BaseInputNode.h"
#include "Components/PerspectiveManager.h"
#include "GameFramework/Pawn.h"
#include "EnhancedInputSubsystems.h"
#include "InputTriggers.h"
#include "InputNode.generated.h"

/**
 *
 */
UCLASS(Blueprintable, EditInlineNew, DontCollapseCategories, Category = "StateMachine|Input")
class CRABTOOLSUE5_API UInputNode : public UBaseInputNode
{
	GENERATED_BODY()

	int Trigger;
	int Start;
	int Ongoing;
	int Canceled;
	int Completed;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	bool bAlwaysBound = true;

	UPROPERTY(EditDefaultsOnly, Category = "Input", meta = (AllowPrivateAccess = "true", Bitmask, BitmaskEnum="/Script/EnhancedInput.ETriggerEvent"))
	int Events = 0;

	
	UPROPERTY(EditDefaultsOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	class UInputAction* Action;
	/*
	UPROPERTY(EditDefaultsOnly, Category="Input|Events", meta=(AllowPrivateAccess = "true"))
	bool bTrigger = false;

	UPROPERTY(EditDefaultsOnly, Category="Input|Events", meta=(AllowPrivateAccess = "true"))
	bool bStart = false;

	UPROPERTY(EditDefaultsOnly, Category="Input|Events", meta=(AllowPrivateAccess = "true"))
	bool bOngoing = false;

	UPROPERTY(EditDefaultsOnly, Category="Input|Events", meta=(AllowPrivateAccess = "true"))
	bool bCompleted = false;

	UPROPERTY(EditDefaultsOnly, Category="Input|Events", meta=(AllowPrivateAccess = "true"))
	bool bCanceled = false;

	*/

	UPROPERTY(BlueprintReadOnly, Category = "Input|Events", meta = (AllowPrivateAccess = "true"))
	bool bHasStarted = false;

	UPROPERTY(BlueprintReadOnly, Category = "Input|Events", meta = (AllowPrivateAccess = "true"))
	FInputActionValue RecentValue;

public:

	virtual void Initialize_Inner_Implementation() override;
	virtual void Enter_Inner_Implementation() override;
	virtual void Exit_Inner_Implementation() override;
	
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StateMachine|Input")
	void TriggerCallback(const FInputActionValue& Value);
	virtual void TriggerCallback_Implementation(const FInputActionValue& Value);
	void TriggerCallback_Internal(const FInputActionValue& Value);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StateMachine|Input")
	void StartCallback(const FInputActionValue& Value);
	virtual void StartCallback_Implementation(const FInputActionValue& Value);
	void StartCallback_Internal(const FInputActionValue& Value);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StateMachine|Input")
	void OngoingCallback(const FInputActionValue& Value);
	virtual void OngoingCallback_Implementation(const FInputActionValue& Value);
	void OngoingCallback_Internal(const FInputActionValue& Value);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StateMachine|Input")
	void CompletedCallback(const FInputActionValue& Value);
	virtual void CompletedCallback_Implementation(const FInputActionValue& Value);
	void CompletedCallback_Internal(const FInputActionValue& Value);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StateMachine|Input")
	void CanceledCallback(const FInputActionValue& Value);
	virtual void CanceledCallback_Implementation(const FInputActionValue& Value);
	void CanceledCallback_Internal(const FInputActionValue& Value);

	/* Used only when canceled/completed are not used, but start is. */
	void FinishedCallback(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateMachine|Input")
	FVector2D GetVec2D();

	UFUNCTION(BlueprintCallable, Category = "StateMachine|Input")
	APawn* GetPawn() const { return this->PawnOwner;  }

private:
	
	void BindCallbacks();
	void UnbindCallbacks();
};