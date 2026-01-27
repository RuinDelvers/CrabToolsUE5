#pragma once

#include "CoreMinimal.h"
#include "RPGProperty.generated.h"

class URPGComponent;
class URPGOperation;



/* Base class for all RPG type properties.*/
UCLASS(Abstract, NotBlueprintType, Blueprintable, DefaultToInstanced, EditInlineNew)
class URPGProperty : public UObject
{
	GENERATED_BODY()

private:

	UPROPERTY(Transient)
	TObjectPtr<URPGComponent> Owner;

public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRPGPropertyChanged, URPGProperty*, Prop);

	UPROPERTY(BlueprintAssignable, Category="RPGProperty")
	FRPGPropertyChanged OnPropertyChanged;

public:

	void Initialize(URPGComponent* Component);

	UFUNCTION(BlueprintCallable, Category="RPGProperty")
	URPGComponent* GetOwner() const { return this->Owner; }

	UFUNCTION(BlueprintNativeEvent, Category="RPGProperty")
	void Initialize_Inner();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "RPGProperty")
	FText GetDisplayText() const;

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
class CRABTOOLSUE5_API URPGResource : public URPGProperty
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resources",
		meta = (GetOptions = "GetAttributeOptions"))
	FName MinimumRef;

	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = "Resources")
	TObjectPtr<URPGProperty> Minimum;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resources",
		meta = (GetOptions = "GetAttributeOptions"))
	FName MaximumRef;

	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = "Resources")
	TObjectPtr<URPGProperty> Maximum;

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Resource")
	float GetPercent() const;
	virtual float GetPercent_Implementation() const { return 0.0f; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Resource")
	void UseResourceInt(int Amount);
	virtual void UseResourceInt_Implementation(int Amount) {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Resource")
	void UseResourceFloat(float Amount);
	virtual void UseResourceFloat_Implementation(float Amount) {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Resource")
	bool HasResourceInt(int Compare) const;
	virtual bool HasResourceInt_Implementation(int Compare) const { return false; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Resource")
	bool HasResourceFloat(float Compare) const;
	virtual bool HasResourceFloat_Implementation(float Compare) const { return false; }

	void SetMinRef(FName Ref) { this->MinimumRef = Ref; }
	void SetMaxRef(FName Ref) { this->MaximumRef = Ref; }

	virtual void SetMinProp(URPGProperty* Prop) {}
	virtual void SetMaxProp(URPGProperty* Prop) {}

protected:

	UFUNCTION()
	TArray<FString> GetAttributeOptions() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintPure, Category = "Getter")
	TSubclassOf<URPGProperty> GetBoundsType() const;
	virtual TSubclassOf<URPGProperty> GetBoundsType_Implementation() const { return nullptr; }
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