#pragma once

#include "Components/RPGSystem/RPGProperty.h"
#include "FlagProperty.generated.h"

UINTERFACE(Blueprintable)
class UBoolRPGProperty : public UInterface
{
	GENERATED_BODY()
};

class IBoolRPGProperty
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, Category = "RPGProperty|Bool")
	bool GetBoolValue() const;
	virtual bool GetBoolValue_Implementation() const { return false; }

	UFUNCTION(BlueprintNativeEvent, Category = "RPGProperty|Bool")
	void SetBoolValue(bool bNewValue);
	virtual void SetBoolValue_Implementation(bool bNewValue) {}
};

UCLASS(BlueprintType)
class CRABTOOLSUE5_API UFlagProperty: public URPGProperty, public IBoolRPGProperty
{
	GENERATED_BODY()


private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Property",
		meta=(AllowPrivateAccess))
	bool bFlag = false;

public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFlagChanged, UFlagProperty*, Flag);

	UPROPERTY(BlueprintAssignable, Category = "RPGProperty")
	FFlagChanged OnFlagChanged;

public:

	UFUNCTION(BlueprintCallable, Category="RPGProperty")
	void SetFlag(bool bNewFlag);

	UFUNCTION(BlueprintCallable, Category = "RPGProperty")
	bool GetFlag() const;

};
