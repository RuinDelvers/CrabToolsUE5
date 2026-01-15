#pragma once

#include "IStateMachineLike.generated.h"

/* Interface for objects which look like State Machines data-wise, but not necessarily function-wise.*/
UINTERFACE(MinimalAPI)
class UStateMachineLike : public UInterface
{
    GENERATED_BODY()
};

class IStateMachineLike
{    
    GENERATED_BODY()

public:
    // Condition Getters
    virtual TArray<FString> GetConditionOptions() const { return {}; };
    virtual TArray<FString> GetDataConditionOptions() const { return {}; };
    
    // States and Events
    virtual TArray<FString> GetEventOptions() const { return {}; };
    virtual TArray<FString> GetStateOptions(const UObject* Asker) const { return {}; };
    virtual TArray<FName> GetEmittedEvents() const { return {}; };

    // SubMachine Getters
    virtual TArray<FString> GetMachineOptions() const { return {}; };
    virtual IStateMachineLike* GetSubMachine(FString& Address) const { return nullptr; }
    virtual TArray<FString> GetSubMachineStateOptions(FName SMName) const { return {}; };

    virtual IStateMachineLike* GetSubMachineLike(FName SubMachine) const { return nullptr; }

    // Editor Functions
    virtual void OnModify() {}
};

/* Interface for objects that act like states within a state machine. */
UINTERFACE(MinimalAPI)
class UStateLike : public UInterface
{
    GENERATED_BODY()
};

class IStateLike
{
    GENERATED_BODY()

public:

    virtual TArray<FString> GetEventOptions() const { return {}; };
    virtual void RenameEvent(FName From, FName To) {}
    virtual void DeleteEvent(FName EventName) {}

    /* Returns the list of states that enter this state. */
    virtual TArray<FString> GetEnterStates() const { return {}; }

    /* Returns the list of states that this state can enter. */
    virtual TArray<FString> GetExitStates() const { return {}; }

    virtual IStateMachineLike* GetMachineLike() const { return nullptr; }

    // Editor Functions
    virtual void OnModify() {}
};

UINTERFACE(MinimalAPI)
class UStateNodeLike : public UInterface
{
    GENERATED_BODY()
};

class IStateNodeLike
{
    GENERATED_BODY()


public:

};

UINTERFACE(MinimalAPI)
class UStateTransitionLike : public UInterface
{
    GENERATED_BODY()
};

class IStateTransitionLike
{
    GENERATED_BODY()

public:

    virtual TArray<FString> GetSourceEventOptions() const { return {}; }
};

/* Interface for data objects passed by state machine data piping. */
UINTERFACE(MinimalAPI)
class UStateMachineDataInterface : public UInterface
{
    GENERATED_BODY()
};

class IStateMachineDataInterface
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintNativeEvent, Category="StateMachine|Data",
        meta=(DeterminesOutputType="Type"))
    UObject* FindDataOfType(TSubclassOf<UObject> Type);
    virtual UObject* FindDataOfType_Implementation(TSubclassOf<UObject> Type) { return nullptr; }

    UFUNCTION(BlueprintNativeEvent, Category = "StateMachine|Data",
        meta = (DeterminesOutputType = "Type", DynamicOutputParam="ReturnValue"))
    void FindAllDataOfType(TSubclassOf<UObject> Type, TArray<UObject*>& ReturnValue);
    virtual void FindAllDataOfType_Implementation(TSubclassOf<UObject> Type, TArray<UObject*>& ReturnValue) { }

    UFUNCTION(BlueprintNativeEvent, Category = "StateMachine|Data", meta = (DeterminesOutputType = "Type"))
    TScriptInterface<UInterface> FindDataImplementing(TSubclassOf<UInterface> Type);
    virtual TScriptInterface<UInterface> FindDataImplementing_Implementation(TSubclassOf<UInterface> Type) {return nullptr; }

    UFUNCTION(BlueprintNativeEvent, Category = "StateMachine|Data",
        meta = (DeterminesOutputType = "Type", DynamicOutputParam = "ReturnValue"))
    void FindAllDataImplementing(TSubclassOf<UInterface> Type, TArray<TScriptInterface<UInterface>>& ReturnValue);
    virtual void FindAllDataImplementing_Implementation(TSubclassOf<UInterface> Type, TArray<TScriptInterface<UInterface>>& ReturnValue) { }
};

UCLASS()
class UStateMachineDataHelpers : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable, Category = "StateMachine|Data", meta = (DeterminesOutputType = "Type", ExpandBoolAsExecs="Found"))
    static UObject* FindDataOfType(TSubclassOf<UObject> Type, UObject* Data, bool& Found);
    static UObject* FindDataOfType(TSubclassOf<UObject> Type, UObject* Data);

    template<class T> static T* FindDataOfType(UObject* Data)
    {
        return Cast<T>(FindDataOfType(T::StaticClass(), Data));
    }

    UFUNCTION(BlueprintCallable, Category = "StateMachine|Data",
        meta = (DeterminesOutputType = "Type", DynamicOutputParam = "ReturnValue", ExpandBoolAsExecs = "Found"))
    static void FindAllDataOfType(TSubclassOf<UObject> Type, UObject* Data, TArray<UObject*>& ReturnValue, bool& Found);
    static void FindAllDataOfType(TSubclassOf<UObject> Type, UObject* Data, TArray<UObject*>& ReturnValue);

    template<class T> static void FindAllDataOfType(UObject* Data, TArray<T*>& ReturnValue)
    {
        TArray<UObject*> Found;

        FindAllDataOfType(T::StaticClass(), Data, Found);

        for (auto& Value : Found)
        {
            ReturnValue.Add(Cast<T>(Value));
        }
    }

    UFUNCTION(BlueprintCallable, Category = "StateMachine|Data", meta = (DeterminesOutputType = "Type", ExpandBoolAsExecs = "Found"))
    static TScriptInterface<UInterface> FindDataImplementing(TSubclassOf<UInterface> Type, UObject* Data, bool& Found);
    static TScriptInterface<UInterface> FindDataImplementing(TSubclassOf<UInterface> Type, UObject* Data);

    template<class T> static TScriptInterface<T> FindDataImplementing(UObject* Data)
    {
        auto Value = FindDataImplementing(T::StaticClass(), Data);      

        return TScriptInterface<T>(Value.GetObjectRef());
    }

    UFUNCTION(BlueprintCallable, Category = "StateMachine|Data",
        meta = (DeterminesOutputType = "Type", DynamicOutputParam = "ReturnValue", ExpandBoolAsExecs = "Found"))
    static void FindAllDataImplementing(TSubclassOf<UInterface> Type, UObject* Data, TArray<TScriptInterface<UInterface>>& ReturnValue, bool& Found);
    static void FindAllDataImplementing(TSubclassOf<UInterface> Type, UObject* Data, TArray<TScriptInterface<UInterface>>& ReturnValue);

    template<class T> static void FindAllDataImplementing(UObject* Data, TArray<TScriptInterface<T>>& ReturnValue)
    {
        TArray<TScriptInterface<UInterface>> Found;

        FindAllDataImplementing(T::StaticClass(), Data, Found);

        for (auto& Value : Found)
        {
            ReturnValue.Add(TScriptInterface<T>(Value.GetObjectRef()));
        }
    }
};