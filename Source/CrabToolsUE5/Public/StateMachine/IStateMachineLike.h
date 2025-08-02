#pragma once

#include "StateMachine/StateMachineEnum.h"
#include "StateMachine/Logging.h"
#include "IStateMachineLike.generated.h"

struct FSMPropertySearch;

struct FSMPropertyReference
{
    FProperty* PropertyRef = nullptr;
    TObjectPtr<UObject> StateMachine;

    template <class T> T* GetValue() const
    {
        if (this->PropertyRef && this->StateMachine)
        {
            if (this->PropertyRef->GetClass() == FObjectProperty::StaticClass())
            {
                return *this->PropertyRef->ContainerPtrToValuePtr<T*>(this->StateMachine);
            }
            if (this->PropertyRef->GetClass() == FStructProperty::StaticClass())
            {
                return this->PropertyRef->ContainerPtrToValuePtr<T>(this->StateMachine);
            }
            else if (this->PropertyRef->GetClass() == FBoolProperty::StaticClass())
            {
                return this->PropertyRef->ContainerPtrToValuePtr<T>(this->StateMachine);
            }
            else
            {
                return this->PropertyRef->ContainerPtrToValuePtr<T>(this->StateMachine);
            }
        }

        return nullptr;
    }

    template <class T> void SetValue(T Value) const
    {
        if (auto BField = CastField<FBoolProperty>(this->PropertyRef))
        {
            BField->SetPropertyValue_InContainer(this->StateMachine, Value);
        }
        else
        {
            this->PropertyRef->SetValue_InContainer(this->StateMachine, &Value);
        }
    }

    bool IsValid() const { return this->PropertyRef != nullptr; }

    operator bool() { return this->IsValid(); }
};

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

    // Property Getters
    #if WITH_EDITOR
        virtual TArray<FString> GetPropertiesOptions(const FSMPropertySearch& SearchParam) const { return {}; }
    #endif //WITH_EDITOR
    virtual FSMPropertyReference GetStateMachineProperty(FString& Address) const { return FSMPropertyReference(); }
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

    #if WITH_EDITOR
        virtual TArray<FString> GetPropertiesOptions(const FSMPropertySearch& SearchParam) const { return {}; }
    #endif //WITH_EDITOR
    virtual FSMPropertyReference GetStateMachineProperty(FString& Address) const { return FSMPropertyReference(); }
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


struct CRABTOOLSUE5_API FSMPropertySearch
{

private:

    FFieldClass* FClass = nullptr;
    UScriptStruct* Struct = nullptr;
    UClass* Class = nullptr;
    UFunction* FunctionSignature = nullptr;

public:
    static FSMPropertySearch ObjectProperty(UClass* Class);
    static FSMPropertySearch StructProperty(UScriptStruct* Struct);
    static FSMPropertySearch Property(FFieldClass* FieldClass);
    static FSMPropertySearch InlineDelegate(UFunction* Signature);

    #if WITH_EDITOR
        bool Matches(FProperty* F) const;
    #endif

    /* Used to get a property from a state machine at a given address, but with many checks. */
    template <class S> FSMPropertyReference GetProperty(S* Node, FString& Address)
    {
        FSMPropertyReference Ref;

        if (Address.StartsWith("::State::"))
        {
            FString Cut = Address.RightChop(9);

            Ref = Node->GetState()->GetStateMachineProperty(Cut);
        }
        else
        {
            Ref = Node->GetMachine()->GetStateMachineProperty(Address);
        }

        FProperty* Prop = Ref.PropertyRef;

        if (Prop)
        {
            if (!Prop->GetClass()->IsChildOf(this->FClass))
            {
                UE_LOG(LogStateMachine, Error, TEXT("Attempted get property of class %s, but found %s"),
                    *this->FClass->GetName(),
                    *Prop->GetClass()->GetName());
                return FSMPropertyReference();
            }

            if (this->Class)
            {
                FObjectProperty* ObjProp = (FObjectProperty*)Prop;

                if (ObjProp->PropertyClass != this->Class)
                {
                    UE_LOG(LogStateMachine, Error, TEXT("Attempted get object property of type %s, but found %s"),
                        *this->Class->GetName(),
                        *ObjProp->PropertyClass->GetName());

                    return FSMPropertyReference();
                }
            }
            else if (this->Struct)
            {
                FStructProperty* StructProp = (FStructProperty*)Prop;

                if (StructProp->Struct != this->Struct)
                {
                    UE_LOG(LogStateMachine, Error, TEXT("Attempted get struct property of type %s, but found %s"),
                        *this->Struct->GetName(),
                        *StructProp->Struct->GetName());

                    return FSMPropertyReference();
                }
            }
        }
        else
        {
            return FSMPropertyReference();
        }

        return Ref;
    }
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