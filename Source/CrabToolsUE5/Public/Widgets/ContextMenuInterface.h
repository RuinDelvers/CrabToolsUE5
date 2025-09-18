#pragma once

#include "CoreMinimal.h"
#include "ContextMenuInterface.generated.h"

USTRUCT(BlueprintType)
struct FContextMenuDisplayData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ContextMenu", meta = (AllowPrivateAccess))
    FText DisplayText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ContextMenu", meta = (AllowPrivateAccess))
    FText Category;
};

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class UContextMenuEntry : public UObject
{
    GENERATED_BODY()

private:

    UPROPERTY(EditAnywhere, Category="ContextMenu", meta=(AllowPrivateAccess))
    TSet<FName> FunctionCallback;

    UPROPERTY(EditAnywhere, Category = "ContextMenu", meta = (AllowPrivateAccess, ShowOnlyInnerProperties))
    FContextMenuDisplayData Data;

public:

    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FContextMenuDelegate);
    DECLARE_MULTICAST_DELEGATE(FContextMenuDelegateNative);

    UPROPERTY(BlueprintAssignable, Category="ContextMenu")
    FContextMenuDelegate OnAction;

    FContextMenuDelegateNative OnActionNative;

public:

    UFUNCTION(BlueprintCallable, Category="ContextMenu")
    void PerformActions();

    UFUNCTION(BlueprintCallable, Category="ContextMenu")
    void SetDisplayData(const FContextMenuDisplayData& NewDisplayData) { this->Data = NewDisplayData; }

    UFUNCTION(BlueprintCallable, Category = "ContextMenu")
    const FContextMenuDisplayData& GetDisplayData() const { return this->Data; }

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
    TArray<UContextMenuEntry*> GatherEntries(AActor* Requester = nullptr);
    virtual TArray<UContextMenuEntry*> GatherEntries_Implementation(AActor* Requester = nullptr) { return {}; }
};