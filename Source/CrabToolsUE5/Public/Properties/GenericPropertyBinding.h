#pragma once

#include "GenericPropertyBinding.generated.h"

UENUM()
enum class EGenericPropertyType
{
	UNKNOWN UMETA(DisplayName = "Unknown"),
	OBJECT_TYPE UMETA(DisplayName="Object"),
	BOOL_TYPE UMETA(DisplayName = "Bool"),
};

UCLASS(BlueprintType, MinimalAPI, EditInlineNew, DefaultToInstanced, CollapseCategories)
class UGenericPropertyBinding : public UObject
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_DELEGATE_RetVal(UObject*, FObjectReturnValue);
	mutable FObjectReturnValue CachedFunctionCall;

	DECLARE_DYNAMIC_DELEGATE_RetVal(bool, FBoolReturnValue);
	mutable FBoolReturnValue CachedBoolCall;

	union
	{
		TWeakObjectPtr<UObject> Object = nullptr;
		bool Bool;
		int Int;
	} mutable CachedValues;
	mutable bool bCachedValues = false;

	// Cached property is always cached, because it depends on the type, not the object.
	mutable FProperty* CachedProperty = nullptr;

	UPROPERTY(EditAnywhere, Category="Properties", meta=(AllowPrivateAccess, GetOptions="GetPropertyOptions"))
	FName PropertyName;

	/* Object to use instead of outer*/
	UPROPERTY(EditAnywhere, Category = "Properties", meta = (AllowPrivateAccess, EditCondition="bIsRoot", EditConditionHides))
	TSubclassOf<UObject> RootObject;

	UPROPERTY(EditAnywhere, Category = "Properties",
		meta = (
			AllowPrivateAccess,
			EditCondition = "!bIsRoot && PropertyType == EGenericPropertyType::OBJECT_TYPE",
			EditConditionHides))
	TSubclassOf<UObject> CastAs;

	UPROPERTY(EditAnywhere, Category = "Properties", meta = (AllowPrivateAccess, EditCondition="bCanHavechild", EditConditionHides))
	TObjectPtr<UGenericPropertyBinding> Child;

	UPROPERTY()
	EGenericPropertyType PropertyType = EGenericPropertyType::UNKNOWN;

	/*
	 * Whether or not to cache the read values. This can avoid nullptr issues, but the data will
	 * not update when its source is updated.
	 */
	UPROPERTY(EditAnywhere, Category = "Properties", meta = (AllowPrivateAccess))
	bool bCacheData = false;

	#if WITH_EDITORONLY_DATA
		UPROPERTY()
		bool bCanHaveChild = false;

		UPROPERTY()
		bool bIsRoot = true;
	#endif

public:

	bool IsObjectProperty() const;
	bool IsFunctionProperty() const;
	bool IsFunctionReturnObjectProperty() const;

	void ClearCache();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Properties")
	UObject* GetObject() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Properties")
	bool GetBool() const;

protected:

	UClass* GetSearchClass() const;
	UClass* GetResultClass() const;

	#if WITH_EDITOR
		virtual void PostEditChangeProperty(FPropertyChangedEvent& Event) override;
		virtual void PostDuplicate(bool DuplicateForPIE) override;
		void IterateFields(UClass* Class, TArray<FString>& Names) const;
		void IterateFunctions(UClass* Class, TArray<FString>& Names) const;

		UFUNCTION()
		TArray<FString> GetPropertyOptions() const;
	#endif

private:

	void SetCachedProperty() const;

	UObject* GetObject_Local(UObject* SourceObject, bool bRecurse=true) const;
	bool GetBool_Local(UObject* SourceObject) const;
	
};