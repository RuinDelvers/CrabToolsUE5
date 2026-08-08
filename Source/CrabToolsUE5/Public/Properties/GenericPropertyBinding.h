#pragma once

#include "Containers/Union.h"
#include "GenericPropertyBinding.generated.h"

DECLARE_DYNAMIC_DELEGATE(FObjectFunctionCall);
DECLARE_DYNAMIC_DELEGATE_RetVal(UObject*, FObjectReturnValue);
DECLARE_DYNAMIC_DELEGATE_RetVal(bool, FBoolReturnValue);
DECLARE_DYNAMIC_DELEGATE_RetVal(int, FIntReturnValue);
DECLARE_DYNAMIC_DELEGATE_RetVal(float, FFloatReturnValue);
DECLARE_DYNAMIC_DELEGATE_RetVal(double, FDoubleReturnValue);

namespace Helpers
{
	template <class T> constexpr T DefaultValue = 0;
	template <> constexpr bool DefaultValue<bool> = false;
	template <> constexpr bool DefaultValue<int> = 0;
	template <> constexpr bool DefaultValue<float> = 0.0f;
	template <> constexpr bool DefaultValue<double> = 0.0;
}

USTRUCT(BlueprintType)
struct CRABTOOLSUE5_API FGenericObjectPropertyPath
{
	GENERATED_BODY()

public:

	// Binding for a function that would return the object we desire.
	//FObjectReturnValue ObjectCallback;

	UPROPERTY()
	bool bIsFunction = false;

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
	virtual void PushProperty(FProperty* Prop);
	virtual void PushProperty(UFunction* Fn);
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

USTRUCT()
struct CRABTOOLSUE5_API FGenericValuePropertyBinding : public FGenericObjectPropertyBinding
{
	GENERATED_BODY()

public:

	UPROPERTY()
	bool bIsFunction = false;

	UPROPERTY()
	FName PropertyName;

	UPROPERTY()
	TObjectPtr<UStruct> RequiredType;

public:

	virtual bool IsEmpty() const override;
	virtual void Pop() override;
	virtual bool CanFinalize() const override;
	virtual FString GetPathString() const override;
	virtual FString GetDisplayString() const override;
	virtual void PushProperty(FProperty* Prop) override;
	virtual void PushProperty(UFunction* Fn) override;

protected:

	template <typename T> struct TypeToProperty { using Type = T; };
	template <> struct TypeToProperty<bool> { using Type = FBoolProperty; };
	template <> struct TypeToProperty<int> { using Type = FIntProperty; };
	template <> struct TypeToProperty<float> { using Type = FFloatProperty; };
	template <> struct TypeToProperty<double> { using Type = FDoubleProperty; };
	template <typename T> using MappedType = typename TypeToProperty<T>::Type;

	template <typename T> struct TypeToDelegate { using Type = T; };
	template <> struct TypeToDelegate<bool> { using Type = FBoolReturnValue; };
	template <> struct TypeToDelegate<int> { using Type = FIntReturnValue; };
	template <> struct TypeToDelegate<float> { using Type = FFloatReturnValue; };
	template <> struct TypeToDelegate<double> { using Type = FDoubleReturnValue; };
	template <typename T> using MappedDelegate = typename TypeToDelegate<T>::Type;

	template <class T> bool ValidPropertyHelper(FProperty* Prop) const
	{
		return Prop->IsA<MappedType<T>>() || FGenericObjectPropertyBinding::ValidProperty(Prop);
	}

	template <class Return> Return GetHelper(void* Source) const
	{
		check(!this->PropertyName.IsNone());

		if (void* Container = FGenericObjectPropertyBinding::Get<void>(Source))
		{
			if (this->bIsFunction)
			{
				MappedDelegate<Return> Callback;
				Callback.BindUFunction(static_cast<UObject*>(Container), this->PropertyName);

				return Callback.Execute();
			}
			else if (auto PropValue = CastFieldChecked<MappedType<Return>>(this->RequiredType->FindPropertyByName(PropertyName)))
			{
				return *PropValue->ContainerPtrToValuePtr<Return>(Source);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Invalid property set for Boolean Binding."));
			}
		}

		return Helpers::DefaultValue<Return>;
	}

	template <class Input> void SetHelper(void* Source, Input SetValue) const
	{
		check(!this->PropertyName.IsNone());
		check(this->Properties.bCanWrite);
		check(!this->bIsFunction);

		void* Container = FGenericObjectPropertyBinding::Get<void>(Source);
		auto PropValue = CastFieldChecked<MappedType<Input>>(this->RequiredType->FindPropertyByName(PropertyName));

		PropValue->SetPropertyValue_InContainer(Container, SetValue);
	}

};

USTRUCT(BlueprintType)
struct CRABTOOLSUE5_API FGenericBoolPropertyBinding: public FGenericValuePropertyBinding
{
	GENERATED_BODY()

public:

	virtual ~FGenericBoolPropertyBinding() {}

	bool Get(const void* Source) const;
	bool Get(void* Source) const;
	void Set(void* Source, bool Value);

	virtual bool ValidProperty(FProperty* CheckProp) const override;
};

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

USTRUCT(BlueprintType)
struct CRABTOOLSUE5_API FGenericIntPropertyBinding : public FGenericValuePropertyBinding
{
	GENERATED_BODY()

public:

	virtual ~FGenericIntPropertyBinding() {}

	int Get(const void* Source) const;
	int Get(void* Source) const;
	void Set(void* Source, int Value);

	virtual bool ValidProperty(FProperty* CheckProp) const override;
};

USTRUCT(BlueprintType)
struct CRABTOOLSUE5_API FGenericFloatPropertyBinding : public FGenericValuePropertyBinding
{
	GENERATED_BODY()

public:

	virtual ~FGenericFloatPropertyBinding() {}

	float Get(const void* Source) const;
	float Get(void* Source) const;
	void Set(void* Source, float Value);

	virtual bool ValidProperty(FProperty* CheckProp) const override;
};


USTRUCT(BlueprintType)
struct CRABTOOLSUE5_API FGenericDoublePropertyBinding : public FGenericValuePropertyBinding
{
	GENERATED_BODY()

public:

	virtual ~FGenericDoublePropertyBinding() {}

	double Get(const void* Source) const;
	double Get(void* Source) const;
	void Set(void* Source, double Value);

	virtual bool ValidProperty(FProperty* CheckProp) const override;
};

