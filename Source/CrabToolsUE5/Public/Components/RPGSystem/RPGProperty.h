#pragma once

#include "CoreMinimal.h"
#include "RPGProperty.generated.h"

class URPGComponent;
class URPGOperation;

DECLARE_DYNAMIC_DELEGATE_OneParam(FRPGPropertyChanged, URPGProperty*, Prop);

/* Base class for all RPG type properties.*/
UCLASS(Abstract, NotBlueprintType, Blueprintable, DefaultToInstanced, EditInlineNew)
class URPGProperty : public UObject
{
	GENERATED_BODY()

private:

	UPROPERTY(Transient)
	TObjectPtr<URPGComponent> Owner;

public:

	void Initialize(URPGComponent* Component);

	UFUNCTION(BlueprintCallable, Category="RPGProperty")
	URPGComponent* GetOwner() const { return this->Owner; }

	UFUNCTION(BlueprintNativeEvent, Category="RPGProperty")
	void Initialize_Inner();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "RPGProperty")
	FText GetDisplayText() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "RPGProperty")
	void ListenToProperty(const FRPGPropertyChanged& Callback);
	virtual void ListenToProperty_Implementation(const FRPGPropertyChanged& Callback) {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "RPGProperty")
	void StopListeningToProperty(UObject* Obj);
	virtual void StopListeningToProperty_Implementation(UObject* Obj) {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintPure, Category="Getter")
	TSubclassOf<URPGSetter> GetSetter() const;
	virtual TSubclassOf<URPGSetter> GetSetter_Implementation() const { return nullptr; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintPure, Category = "Getter")
	TSubclassOf<URPGCompare> GetCompare() const;
	virtual TSubclassOf<URPGCompare> GetCompare_Implementation() const { return nullptr; }

protected:

	virtual FText GetDisplayText_Implementation() const;
	virtual void Initialize_Inner_Implementation();
};

/*
 * Base class for resource types. Resources generally have a minimum and a maximum,
 * and will be kept within those bounds.
 */
UCLASS(Abstract, NotBlueprintType, Blueprintable, DefaultToInstanced, EditInlineNew)
class URPGResource : public URPGProperty
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, Category = "RPGProperty",
		meta = (GetOptions = "GetAttributeOptions"))
	FName MinimumRef;

	UPROPERTY(EditAnywhere, Category = "RPGProperty",
		meta = (GetOptions = "GetAttributeOptions"))
	FName MaximumRef;

public:

	void SetMinRef(FName Ref) { this->MinimumRef = Ref; }
	void SetMaxRef(FName Ref) { this->MaximumRef = Ref; }

	virtual void SetMinProp(URPGProperty* Prop) {}
	virtual void SetMaxProp(URPGProperty* Prop) {}
};

/*
 * An abstract operation on a property. In contrast to operators, which are for non-destructive
 * effects on attributes, these can be destructive and are designed for use external to different
 * APIs that access RPG Properties. A simple example is setting a value for an IntAttributes
 * or comparing against it in State Machines.
 */
UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class URPGOperation : public UObject
{
	GENERATED_BODY()

protected:

	#if WITH_EDITORONLY_DATA
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPGProperty",
			meta=(EditCondition="!bExternalControlProperty", EditConditionHides))
		TSoftClassPtr<URPGComponent> PropertySource;

		/*
		 * Whether to handle searching for property names internally, or to to let an external
		 * object to control its editing.
		 */
		UPROPERTY()
		bool bExternalControlProperty = false;
	#endif

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="RPGProperty",
		meta=(GetOptions="GetPropertyNames",
			EditCondition = "!bExternalControlProperty", EditConditionHides))
	FName PropertyName;

	UPROPERTY()
	TObjectPtr<URPGComponent> Component;

	UPROPERTY()
	TObjectPtr<URPGProperty> Property;

public:

	UFUNCTION(BlueprintCallable, Category="RPGProperty")
	void Initialize(URPGComponent* InitComponent);

	#if WITH_EDITOR
		void SetExternalControl(
			bool bNewExternalControl,
			TSoftClassPtr<URPGComponent> Comp,
			FName ExternalPropName);
		void SetPropertyName(FName NewPropName) { this->PropertyName = NewPropName; }
	#endif

protected:

	UFUNCTION(BlueprintNativeEvent, Category = "RPGProperty")
	void Initialize_Inner();
	virtual void Initialize_Inner_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, Category="RPGProperty")
	UClass* GetPropertySearchType() const;
	virtual UClass* GetPropertySearchType_Implementation() const { return nullptr; }

	#if WITH_EDITOR
		UFUNCTION()
		virtual TArray<FString> GetPropertyNames() const;
	#endif
};

UCLASS(Abstract, Blueprintable)
class URPGSetter : public URPGOperation
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "RPGProperty")
	void ApplyValue();
	virtual void ApplyValue_Implementation() {}

};

UCLASS(Abstract, Blueprintable)
class URPGCompare : public URPGOperation
{
	GENERATED_BODY()


public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintPure, Category = "RPGProperty")
	bool Compare();
	virtual bool Compare_Implementation() { return false; }

};