#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PropertyManagement.generated.h"

/**
 * Functions for handling the creation of custom default objects and duplicating properties.
 */
UCLASS()
class CRABTOOLSUE5_API UPropertyManagementLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	static TSet<FFieldClass*>& ValidInheritablePropertyTypes();

	/*
	 * Compares the Current object with the Copied Archetype and to the Parent Object and updates
	 * properties in both the copied Archetype and Current Object to match correct default values along
	 * with user changed values. Note this won't copy all properties, just ones which can safely copied.
	 * Note this isn't safe for any object; If an object has a non-property pointer it will be copied
	 * directly and may not be valid.
	 * 
	 * The general procedure for this is as follows: 
	 * 
	 * - If there is a difference in the classes, then the parent is copied to both the current object
	 *   and the archetype.
	 * 
	 * - If the classes for the parent and archetype match, and the class of the current object is a 
	 *   subclass of the archetype, then we proceed to attempt to deep copy properties which exist
	 *   in the archetype.
	 * 
	 * - If a property in the archetype is different from the parent, the property from the parent is
	 *   copied to the archetype. If the property is the same in the current object, then it is
	 *   also copied to the current object.
	 * 
	 * Note, this assumes that if Parent->GetClass() == CopiedArchetype->GetClass() that the are equal
	 * in values. If any values have been changed, then behaviour for this isn't defined.
	 */
	#if WITH_EDITOR
	UFUNCTION(BlueprintCallable, Category="Object")	
	static bool UpdateObjectInheritanceProperties(
		const UObject* Parent,
		UObject*& CopiedArchetype,
		UObject*& CurrentObject);
	#endif //WITH_EIDTOR

	/* Templated version. */
	template <class T> static bool UpdateObjectInheritanceProperties(
		const UObject* Parent,
		TObjectPtr<T>& CopiedArchetype,
		TObjectPtr<T>& CurrentObject)
	{
		UObject* ArchPtr = CopiedArchetype.Get();
		UObject* ChildPtr = CurrentObject.Get();

		bool bChanged = UpdateObjectInheritanceProperties(Parent, ArchPtr, ChildPtr);

		CopiedArchetype = Cast<T>(ArchPtr);
		CurrentObject = Cast<T>(ChildPtr);

		return bChanged;
	}

	static void CopyPropertiesSmart(
		FProperty* Property,
		const UObject* Source,
		UObject* Dest);
};