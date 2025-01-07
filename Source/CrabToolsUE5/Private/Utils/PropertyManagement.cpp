#include "Utils/PropertyManagement.h"
#include "UObject/PropertyOptional.h"

#if WITH_EDITOR
bool UPropertyManagementLibrary::UpdateObjectInheritanceProperties(
		const UObject* Parent,
		UObject*& CopiedArchetype,
		UObject*& CurrentObject)
{
	if (!IsValid(Parent))
	{
		CopiedArchetype = nullptr;
		CurrentObject = nullptr;

		return true;
	}

	bool bChangedProperty = false;

	// If either of the child objects aren't valid, then go ahead and recopy them.
	if (!IsValid(CopiedArchetype) || !IsValid(CurrentObject))
	{
		CopiedArchetype = DuplicateObject(Parent, CopiedArchetype->GetOuter());
		CurrentObject = DuplicateObject(CopiedArchetype, CopiedArchetype->GetOuter());

		bChangedProperty = true;
	}
	// First check for classes, and that they make sense given the objects.
	// If the classes don't match up, then remake the objects entirely.
	else if (CopiedArchetype->GetClass() != Parent->GetClass())
	{
		CopiedArchetype = DuplicateObject(Parent, CopiedArchetype->GetOuter());
		CurrentObject = DuplicateObject(CopiedArchetype, CopiedArchetype->GetOuter());

		bChangedProperty = true;
	} 
	// If the CurrentObject isn't a subclass of the archetype, then it's not possible to correct change the
	// properties safely, and we must reset the situation.
	else if (!CurrentObject->IsA(CopiedArchetype->GetClass()))
	{
		CurrentObject = DuplicateObject(CopiedArchetype, CopiedArchetype->GetOuter());

		bChangedProperty = true;
	}
	else
	{
		// If classes are fine, then we make sure that the Archetype object has the same
		// property values as the Parent. If not, then we need to recopy a new Archetype.

		// If the Archetype has changed at all, then we modify the CurrentObject.
		// We find all the properties that were changed from the CurrentObject to the OldArchetype,
		// and store them to make sure they aren't overwritten.
		// For all other properties, they are deep copied.

		for (TFieldIterator<FProperty> FIT(CopiedArchetype->GetClass(), EFieldIteratorFlags::IncludeSuper); FIT; ++FIT)
		{
			FProperty* f = *FIT;

			if (f->HasMetaData("IgnorePropertySearch")) { continue; }

			if (ValidInheritablePropertyTypes().Contains(f->GetClass()))
			{
				// If the parent object property was changed (i.e. from linker changes), then  we need
				// to inspect the CurrentObject to see if we need to update it's values.
				if (!f->Identical_InContainer(CopiedArchetype, Parent))
				{
					bChangedProperty = true;

					if (f->Identical_InContainer(CopiedArchetype, CurrentObject))
					{
						CopyPropertiesSmart(f, Parent, CurrentObject);
					}

					// Copy from the parent to the archetype if there is a discrepancy. 
					CopyPropertiesSmart(f, Parent, CopiedArchetype);
				}
			}
		}
	}

	return bChangedProperty;
}
#endif // WITH_EDITOR

void UPropertyManagementLibrary::CopyPropertiesSmart(
	FProperty* Property,
	const UObject* Source,
	UObject* Dest)
{
	if (auto ObjSource = CastField<FObjectProperty>(Property))
	{
		TObjectPtr<UObject> SourceRef;
		ObjSource->GetValue_InContainer(Source, &SourceRef);

		// If for some reason the object isn't using the container as an outer somehow, we do not 
		// want to duplicate it, as it doesn't seem to be an owned property.
		if (Source->IsInOuter(SourceRef))
		{
			auto Duplicate = DuplicateObject(SourceRef, Dest);
			Property->SetValue_InContainer(Dest, Duplicate);
		}
	}
	else
	{
		Property->CopyCompleteValue_InContainer(Dest, Source);
	}
}

TSet<FFieldClass*>& UPropertyManagementLibrary::ValidInheritablePropertyTypes()
{
	static TSet<FFieldClass*> ValidClasses =
	{
		FBoolProperty::StaticClass(),
		FEnumProperty::StaticClass(),
		FNumericProperty::StaticClass(),
		FObjectProperty::StaticClass(),
		FObjectPtrProperty::StaticClass(),
		FClassProperty::StaticClass(),
		FClassPtrProperty::StaticClass(),
		FOptionalProperty::StaticClass(),
		FStructProperty::StaticClass(),
	};

	return ValidClasses;
}