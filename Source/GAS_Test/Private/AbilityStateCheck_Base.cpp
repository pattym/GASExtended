
#include "AbilityStateCheck_Base.h"
#include "UObject/Package.h"

/**
 * Retrieves the world context for this object.
 * 
 * If this object is a Class Default Object (CDO), it returns nullptr.
 * Otherwise, it retrieves the world from its outer object.
 * 
 * @return A pointer to the UWorld this object belongs to, or nullptr if it's a CDO.
 */
UWorld* UAbilityStateCheck_Base::GetWorld() const
{
	if (!IsInstantiated()) // Check if this object is a CDO
	{
		// If we are a CDO, return nullptr instead of calling GetOuter()->GetWorld().
		// This prevents UObject::ImplementsGetWorld from falsely detecting a world context.
		return nullptr;
	}

	// Otherwise, retrieve the world context from the outer object.
	return GetOuter()->GetWorld();
}

/**
 * Determines if this object is an instantiated instance (not a Class Default Object).
 */
bool UAbilityStateCheck_Base::IsInstantiated() const
{
	// Check if the object does not have the RF_ClassDefaultObject flag.
	// If the flag is present, this means the object is a CDO.
	return !HasAllFlags(RF_ClassDefaultObject);
}
