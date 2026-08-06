#pragma once

#include "Containers/Union.h"
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

DECLARE_DYNAMIC_DELEGATE_RetVal(UObject*, FObjectReturnValue);

USTRUCT(BlueprintType)
struct CRABTOOLSUE5_API FGenericObjectPropertyPath
{
	GENERATED_BODY()

public:

	// Binding for a function that would return the object we desire.
	FObjectReturnValue ObjectCallback;

	UPROPERTY()
	TObjectPtr<UStruct> SearchType;

	UPROPERTY()
	FName PropertyName;

public:

	void* Get(void* Source) const;
};

USTRUCT(BlueprintType)
struct CRABTOOLSUE5_API FGenericObjectPropertyBindingProperties
{
	GENERATED_BODY()

public:

	static const FGenericObjectPropertyBindingProperties Default;

public:

	UPROPERTY()
	bool bCanWrite = false;
};

USTRUCT(BlueprintType)
struct CRABTOOLSUE5_API FGenericObjectPropertyBinding
{
	GENERATED_BODY()

public:

	UPROPERTY()
	TArray<FGenericObjectPropertyPath> Path;

	UPROPERTY()
	FGenericObjectPropertyBindingProperties Properties;

public:

	FGenericObjectPropertyBinding() {}
	FGenericObjectPropertyBinding(
		UStruct* InitRequiredType,
		const FGenericObjectPropertyBindingProperties& Props = FGenericObjectPropertyBindingProperties::Default)
	: Properties(Props)
	{}

	virtual ~FGenericObjectPropertyBinding() {}

	virtual void Pop();
	virtual bool IsEmpty() const;
	virtual FString GetPathString() const;
	virtual FString GetDisplayString() const;
	virtual void PushProperty(FProperty* Prop, UClass* CastClass = nullptr);
	virtual void WriteToProperty(TSharedPtr<IPropertyHandle> Property) const;
	virtual bool CanFinalize() const;
	bool IsBound() const { return Path.Num() > 0; }
	UStruct* ResultType() const;

	template <class T> T* Get(void* Source) const
	{
		void* Result = Source;
		for (auto& Part : this->Path)
		{
			Result = Part.Get(Result);
		}

		return reinterpret_cast<T*>(Result);
	}

	template <class T> T* Get(const void* Source) const
	{
		return this->Get<T>(const_cast<void*>(Source));
	}

	virtual bool ValidProperty(FProperty* CheckProp) const;
};

USTRUCT(BlueprintType)
struct CRABTOOLSUE5_API FGenericBoolPropertyBinding: public FGenericObjectPropertyBinding
{
	GENERATED_BODY()

public:

	UPROPERTY()
	FName PropertyName;

	UPROPERTY()
	TObjectPtr<UStruct> RequiredType;

public:

	virtual ~FGenericBoolPropertyBinding() {}

	virtual void Pop() override;
	virtual bool IsEmpty() const override;
	virtual bool CanFinalize() const override;
	virtual FString GetPathString() const override;
	virtual FString GetDisplayString() const override;
	virtual void PushProperty(FProperty* Prop, UClass* CastClass=nullptr) override;
	virtual void WriteToProperty(TSharedPtr<IPropertyHandle> Property) const override;

	bool Get(const void* Source) const;
	bool Get(void* Source) const;
	void Set(void* Source, bool Value);

	virtual bool ValidProperty(FProperty* CheckProp) const override;
};

DECLARE_DYNAMIC_DELEGATE(FObjectFunctionCall);

USTRUCT(BlueprintType)
struct CRABTOOLSUE5_API FGenericFunctionPropertyBinding: public FGenericObjectPropertyBinding
{
	GENERATED_BODY()

public:

	UPROPERTY()
	FName PropertyName;

public:

	virtual ~FGenericFunctionPropertyBinding() {}
	void Execute(void* Source);
};
