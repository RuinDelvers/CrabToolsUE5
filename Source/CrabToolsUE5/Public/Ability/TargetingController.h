#pragma once

#include "CoreMinimal.h"
#include "Delegates/Delegate.h"
#include "TargetingController.generated.h"

class ITargetingControllerInterface;

UINTERFACE(MinimalAPI, Blueprintable, BlueprintType)
class UTargetingControllerInterface : public UInterface
{
    GENERATED_BODY()
};

USTRUCT(BlueprintType)
struct FTargetingData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category="Targeting")
	TObjectPtr<AActor> TargetActor;

	UPROPERTY(BlueprintReadWrite, Category = "Targeting")
	FVector TargetLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, Category = "Targeting")
	FVector TargetNormal = FVector::UpVector;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FConfirmTargetsMulti, TScriptInterface<ITargetingControllerInterface>, Targeter);
DECLARE_DYNAMIC_DELEGATE_OneParam(FConfirmTargetsSingle, TScriptInterface<ITargetingControllerInterface>, Targeter);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FValidateTargeting, bool, IsValid, const FText&, Reason);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FValidateTargetingSingle, bool, IsValid, const FText&, Reason);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTargetingUpdatedMulti, AActor*, Targeting);
DECLARE_DYNAMIC_DELEGATE_OneParam(FTargetingUpdated, AActor*, Targeting);

class ITargetingControllerInterface
{
    GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Targeting")
	AActor* GetUsingActor() const;
	virtual AActor* GetUsingActor_Implementation() const { return nullptr; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Targeting")
	void Initialize();
	virtual void Initialize_Implementation() {}

	/* Manually add a target to this controller. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Targeting")
	void AddTarget(const FTargetingData& TargetData);
	virtual void AddTarget_Implementation(const FTargetingData& TargetData) {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Targeting")
	void SetEnabled(bool bNewEnabled);
	virtual void SetEnabled_Implementation(bool bNewEnabled) {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Targeting")
	bool GetEnabled() const;
	virtual bool GetEnabled_Implementation() const { return true; }

	/* Save the currently selected targets */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Targeting")
	void PushTarget();
	virtual void PushTarget_Implementation() {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Targeting")
	void PopTarget();
	virtual void PopTarget_Implementation() {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Targeting")
	void RemoveTarget(int Index);
	virtual void RemoveTarget_Implementation(int Index) {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Targeting")
	int GetTargetCount() const;
	virtual int GetTargetCount_Implementation() const { return 0; }


	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Targeting")
	void Confirm();
	virtual void Confirm_Implementation() {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Targeting")
	void AddListener(const FConfirmTargetsSingle& Callback);
	virtual void AddListener_Implementation(const FConfirmTargetsSingle& Callback) {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Targeting")
	void AddValidationListener(const FValidateTargetingSingle& Callback);
	virtual void AddValidationListener_Implementation(const FValidateTargetingSingle& Callback) {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Targeting")
	void AddDestroyedListener(const FTargetingUpdated& Callback);
	virtual void AddDestroyedListener_Implementation(const FTargetingUpdated& Callback) {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Targeting")
	void AddDisabledListener(const FTargetingUpdated& Callback);
	virtual void AddDisabledListener_Implementation(const FTargetingUpdated& Callback) {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Targeting")
	void GetTargetData(TArray<FTargetingData>& OutData) const;
	virtual void GetTargetData_Implementation(TArray<FTargetingData>& OutData) const { }

	/* Called by implementing targeters to validate the state for confirmation or pushing. */
	UFUNCTION(BlueprintNativeEvent, Category = "Targeting")
	bool Validate(FText& Reason) const;
	virtual bool Validate_Implementation(FText& Reason) const { return true; }
};