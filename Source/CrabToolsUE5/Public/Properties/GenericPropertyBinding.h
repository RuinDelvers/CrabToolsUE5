#pragma once

#include "GenericPropertyBinding.generated.h"

UENUM()
enum class EGenericPropertyType
{
	UNKNOWN UMETA(DisplayName = "Unknown"),
	DELEGATE_TYPE UMETA(DisplayName = "Delegate"),
	OBJECT_TYPE UMETA(DisplayName="Object"),
	STRUCT_TYPE UMETA(DisplayName = "Struct"),
	BOOL_TYPE UMETA(DisplayName = "Bool"),
	INT_TYPE UMETA(DisplayName = "Int"),
	FLOAT_TYPE UMETA(DisplayName = "Float"),
	DOUBLE_TYPE UMETA(DisplayName = "Double"),
};

namespace Helpers
{
	template <class T> constexpr T DefaultValue = 0;
	template <> constexpr bool DefaultValue<bool> = false;
	template <> constexpr bool DefaultValue<int> = 0;

	
}


UCLASS(BlueprintType, MinimalAPI, EditInlineNew, DefaultToInstanced, CollapseCategories)
class UGenericPropertyBinding : public UObject
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_DELEGATE_RetVal(UObject*, FObjectReturnValue);
	DECLARE_DYNAMIC_DELEGATE_RetVal(bool, FBoolReturnValue);
	DECLARE_DYNAMIC_DELEGATE_RetVal(int, FIntReturnValue);
	DECLARE_DYNAMIC_DELEGATE_RetVal(float, FFloatReturnValue);
	DECLARE_DYNAMIC_DELEGATE_RetVal(double, FDoubleReturnValue);

	union InnerContainer
	{
		FObjectReturnValue Object;
		FBoolReturnValue Bool;
		FFloatReturnValue Float;
		FDoubleReturnValue Double;
		FIntReturnValue Int;

		InnerContainer() { }
		~InnerContainer() { }
	} mutable Functions;

	union
	{
		TWeakObjectPtr<UObject> Object = nullptr;
		void* Struct;
		bool Bool;
		int Int;
		float Float;
		double Double;
	} mutable CachedValues;
	mutable bool bCachedValues = false;
	mutable bool bCachedFunction = false;
	bool bParentCachesValue = false;

	FProperty* CachedProperty = nullptr;
	#if WITH_EDITOR
		bool bWasInitialized = false;
	#endif

	UPROPERTY(EditAnywhere, Category="Properties", meta=(AllowPrivateAccess, GetOptions="GetPropertyOptions"))
	FName PropertyName;

	/* Object to use instead of outer*/
	UPROPERTY(EditAnywhere, Category = "Properties", meta = (AllowPrivateAccess, EditCondition="bIsRoot", EditConditionHides))
	TSubclassOf<UObject> RootObjectType;

	UPROPERTY()
	TObjectPtr<UObject> RootObject;

	UPROPERTY(EditAnywhere, Category = "Properties",
		meta = (
			AllowPrivateAccess,
			EditCondition = "PropertyType == EGenericPropertyType::OBJECT_TYPE",
			EditConditionHides))
	TSubclassOf<UObject> CastAs;

	UPROPERTY(EditAnywhere, Category = "Properties", meta = (AllowPrivateAccess, EditCondition="bCanHavechild", EditConditionHides))
	TObjectPtr<UGenericPropertyBinding> Child;

	UPROPERTY()
	EGenericPropertyType PropertyType = EGenericPropertyType::UNKNOWN;

	UPROPERTY()
	bool bIsFunctionType = false;

	/*
	 * Whether or not to cache the read values. This can avoid nullptr issues, but the data will
	 * not update when its source is updated.
	 */
	UPROPERTY(EditAnywhere, Category = "Properties", meta = (AllowPrivateAccess, EditCondition="bCanCacheData", EditConditionHides))
	bool bCacheData = false;

	#if WITH_EDITORONLY_DATA
		UPROPERTY()
		bool bCanCacheData = true;

		UPROPERTY()
		bool bCanHaveChild = false;

		UPROPERTY()
		bool bIsRoot = true;
	#endif

public:

	UFUNCTION(BlueprintCallable, Category="Properties")
	void Initialize();

	bool IsObjectProperty() const;
	bool IsFunctionProperty() const;
	bool IsFunctionReturnObjectProperty() const;
	bool IsStructProperty() const;
	bool IsDelegateProperty() const;

	void ClearCache();

	UFUNCTION(BlueprintCallable, Category = "Properties")
	void SetSource(UObject* NewSource);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Properties")
	FString GetPropertyPath() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Properties")
	UObject* GetObject() const;

	template <class T> T* GetObject() const
	{
		return Cast<T>(this->GetObject());
	}

	/* Returns the pointer to the struct for this binding. */
	void* GetStruct(bool& bFound) const;

	template <class T> T& GetStruct(bool& bFound)
	{
		T* Ptr = reinterpret_cast<T*>(this->GetStruct(bFound));
		return *Ptr;
	}

	void CallFunction();

	UFUNCTION(BlueprintCallable, Category = "Properties")
	void SetObject(UObject* Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Properties")
	bool GetBool() const;

	UFUNCTION(BlueprintCallable, Category = "Properties")
	void SetBool(bool Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Properties")
	int GetInt() const;

	UFUNCTION(BlueprintCallable, Category = "Properties")
	void SetInt(int Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Properties")
	float GetFloat() const;

	UFUNCTION(BlueprintCallable, Category = "Properties")
	void SetFloat(float Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Properties")
	double GetDouble() const;

	UFUNCTION(BlueprintCallable, Category = "Properties")
	void SetDouble(double Value);

	UObject* GetRootObject() const;

protected:

	bool IsContainerType() const;

	UClass* GetSearchClass() const;
	UClass* GetResultClass() const;
	UObject* GetSourceObject() const;

	#if WITH_EDITOR
		virtual void PostEditChangeProperty(FPropertyChangedEvent& Event) override;
		virtual void PostDuplicate(bool DuplicateForPIE) override;
		virtual void PostLoad() override;
		void IterateFields(UClass* Class, TArray<FString>& Names) const;
		void IterateFunctions(UClass* Class, TArray<FString>& Names) const;

		UFUNCTION()
		TArray<FString> GetPropertyOptions() const;		
	#endif

	void NotInitializedError() const;
	void NullSourceError() const;

private:

	void ValidateData();
	void SetCachedProperty();

	void* GetContainer_Local(void* SourceObject, bool bRecurse=false) const;
	void SetObject_Local(void* SourceObject, UObject* Value);

	void CallFunction_Local(void* SourceObject);

	template <class Return> Return GenericLocalPropGet(void* SourceObject) const
	{
		if (!SourceObject)
		{
			this->NullSourceError();
			return Helpers::DefaultValue<Return>;
		}
		#if WITH_EDITOR
			if (!this->bWasInitialized)
			{
				this->NotInitializedError();
				return Helpers::DefaultValue<Return>;
			}
		#endif
		if (this->bCachedValues)
		{
			return this->GetCachedValue<Return>();
		}

		if (this->IsContainerType())
		{
			return this->Child->GenericLocalPropGet<Return>(
				this->GetContainer_Local(SourceObject));
		}
		else
		{
			if (this->bIsFunctionType)
			{
				Return Value = this->BindAndCall<Return>(SourceObject);
				this->SetCachedValue<Return>(Value);

				return Value;
			}
			else
			{
				Return Output = *this->GetProperty<MappedType<Return>>()->ContainerPtrToValuePtr<Return>(SourceObject);
				
				this->SetCachedValue<Return>(Output);

				return Output;
			}
		}

		return Helpers::DefaultValue<Return>;
	}

	template <class Type> void GenericLocalPropSet(
		void* SourceObject,
		Type Value)
	{
		#if WITH_EDITOR
			if (!this->bWasInitialized)
			{
				this->NotInitializedError();
				return;
			}
		#endif

		switch (this->PropertyType)
		{		
			case EGenericPropertyType::OBJECT_TYPE:
				this->Child->GenericLocalPropSet<Type>(
					this->GetContainer_Local(SourceObject),
					Value);
				break;
			case EGenericPropertyType::STRUCT_TYPE:
				break;
			default:
				this->GetProperty<MappedType<Type>>()->SetPropertyValue_InContainer(SourceObject, Value);
				break;
		}
	}	

	template <class T> T GetCachedValue() const {}
	template <> UObject* GetCachedValue<UObject*>() const { return this->CachedValues.Object.Get(); }
	template <> void* GetCachedValue<void*>() const { return this->CachedValues.Struct; }
	template <> bool GetCachedValue<bool>() const { return this->CachedValues.Bool; }
	template <> int GetCachedValue<int>() const { return this->CachedValues.Int; }
	template <> float GetCachedValue<float>() const { return this->CachedValues.Float; }
	template <> double GetCachedValue<double>() const { return this->CachedValues.Double; }

	template <class T> void SetCachedValue(T Value) const {}
	template <> void SetCachedValue<UObject*>(UObject* Value) const { if (this->bCacheData) { this->CachedValues.Object = Value; this->bCachedValues = true; } }
	template <> void SetCachedValue<void*>(void* Value) const { if (this->bCacheData) { this->CachedValues.Struct = Value; this->bCachedValues = true; } }
	template <> void SetCachedValue<bool>(bool Value) const { if (this->bCacheData) { this->CachedValues.Bool = Value; this->bCachedValues = true; } }
	template <> void SetCachedValue<int>(int Value) const { if (this->bCacheData) { this->CachedValues.Int = Value; this->bCachedValues = true; } }
	template <> void SetCachedValue<float>(float Value) const { if (this->bCacheData) { this->CachedValues.Float = Value; this->bCachedValues = true; } }
	template <> void SetCachedValue<double>(double Value) const { if (this->bCacheData) { this->CachedValues.Double = Value; this->bCachedValues = true; } }

	template <typename T> struct TypeToPropMap { using Type = void; };
	template <> struct TypeToPropMap<UObject*> { using Type = FObjectProperty; };
	template <> struct TypeToPropMap<bool> { using Type = FBoolProperty; };
	template <> struct TypeToPropMap<int> { using Type = FIntProperty; };
	template <> struct TypeToPropMap<float> { using Type = FFloatProperty; };
	template <> struct TypeToPropMap<double> { using Type = FDoubleProperty; };

	template <typename T> struct TypeToDelegateMap { using Type = void; };
	template <> struct TypeToDelegateMap<UObject*> { using Type = FObjectReturnValue; };
	template <> struct TypeToDelegateMap<bool> { using Type = FBoolReturnValue; };
	template <> struct TypeToDelegateMap<int> { using Type = FIntReturnValue; };
	template <> struct TypeToDelegateMap<float> { using Type = FFloatReturnValue; };
	template <> struct TypeToDelegateMap<double> { using Type = FDoubleReturnValue; };

	template <typename T> using MappedType     = typename TypeToPropMap<T>::Type;
	template <typename T> using MappedDelegate = typename TypeToDelegateMap<T>::Type;

	template <class T> void CacheCallback(MappedDelegate<T> F) const {}
	template <> void CacheCallback<UObject*>(MappedDelegate<UObject*> F) const { this->Functions.Object = F; }
	template <> void CacheCallback<bool>(MappedDelegate<bool> F) const { this->Functions.Bool = F; }
	template <> void CacheCallback<int>(MappedDelegate<int> F) const { this->Functions.Int = F; }
	template <> void CacheCallback<float>(MappedDelegate<float> F) const { this->Functions.Float = F; }
	template <> void CacheCallback<double>(MappedDelegate<double> F) const { this->Functions.Double = F; }

	template <class T> T CallDelegate() const {}
	template <> UObject* CallDelegate<UObject*>() const { return this->Functions.Object.Execute(); }
	template <> bool CallDelegate<bool>() const { return this->Functions.Bool.Execute(); }
	template <> int CallDelegate<int>() const { return this->Functions.Int.Execute(); }
	template <> float CallDelegate<float>() const { return this->Functions.Float.Execute(); }
	template <> double CallDelegate<double>() const { return this->Functions.Double.Execute(); }

	template <class T> T BindAndCall(void* Source) const
	{
		if (this->bParentCachesValue && this->bCachedFunction)
		{
			return this->CallDelegate<T>();
		}
		else
		{
			MappedDelegate<T> Callback;
			Callback.BindUFunction((UObject*)Source, this->PropertyName);

			if (this->bParentCachesValue)
			{
				this->CacheCallback<T>(Callback);
			}

			return Callback.Execute();
		}
	}

	template <class Property> inline Property* GetProperty() const
	{
		#if WITH_EDITOR
			return CastFieldChecked<Property>(this->CachedProperty);
		#else
			return (Property*)this->CachedProperty;
		#endif
	}
};