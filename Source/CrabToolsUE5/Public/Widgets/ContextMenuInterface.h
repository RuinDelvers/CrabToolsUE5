#pragma once

#include "CoreMinimal.h"
#include "ContextMenuInterface.generated.h"

UCLASS(EditInlineNew, DefaultToInstanced)
class UContextMenuEntry : public UObject
{
    GENERATED_BODY()

private:

    UPROPERTY(EditAnywhere, Category="ContextMenu", meta=(AllowPrivateAccess))
    TSet<FName> FunctionCallback;

    UPROPERTY()
    bool bHasBeenBound = false;

public:

    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FContextMenuDelegate);

    UPROPERTY(BlueprintAssignable, Category="ContextMenu")
    FContextMenuDelegate OnAction;


public:

    void PerformActions();

protected:

    UFUNCTION()
    TArray<FString> GetFunctionOptions() const;
};

UINTERFACE(MinimalAPI, Blueprintable, BlueprintType)
class UContextMenuInterface : public UInterface
{
    GENERATED_BODY()
};

class IContextMenuInterface
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="ContextMenu")
    void GatherEntries();
};