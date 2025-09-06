#pragma once

#include "Components/ActorComponent.h"
#include "UIDComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UUIDComponent : public UActorComponent
{
	GENERATED_BODY()

private:

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, DuplicateTransient, Category = "UID", meta = (AllowPrivateAccess, EditCondition = "false"))
	FGuid ActorUID;

public:

	virtual void OnComponentCreated() override;

	UFUNCTION(BlueprintCallable, Category="UID")
	FGuid GetUID() const { return this->ActorUID; }

private:

	UFUNCTION()
	void Callback();
};