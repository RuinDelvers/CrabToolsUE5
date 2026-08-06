#pragma once
 
#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "StateMachine/StateMachine.h"
#include "StateMachineBlueprintFactory.generated.h"
 
UCLASS(HideCategories=Object, MinimalAPI)
class UStateMachineBlueprintFactory : public UFactory
{
	GENERATED_BODY()

private:
	
	// The type of blueprint that will be created
	UPROPERTY(EditAnywhere, Category="PSMBlueprintFactory")
	TEnumAsByte<enum EBlueprintType> BlueprintType;

	// The parent class of the created blueprint
	UPROPERTY(EditAnywhere, Category="PSMBlueprintFactory", meta=(AllowAbstract = "")) 
	TSubclassOf<class UStateMachine> ParentClass;

	UPROPERTY(Transient)
	TObjectPtr<UClass> RootMachineClass;

public:
	UStateMachineBlueprintFactory();

	virtual bool ConfigureProperties() override;

	virtual bool ShouldShowInNewMenu() const override;

	virtual UObject* FactoryCreateNew(
		UClass* Class, 
		UObject* InParent, 
		FName Name, 
		EObjectFlags Flags, 
		UObject* Context, 
		FFeedbackContext* Warn, 
		FName CallingContext) override;

	virtual UObject* FactoryCreateNew(
		UClass* Class, 
		UObject* InParent, 
		FName Name, 
		EObjectFlags Flags, 
		UObject* Context, 
		FFeedbackContext* Warn) override;
};