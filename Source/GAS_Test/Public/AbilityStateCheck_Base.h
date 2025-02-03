#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "GameplayTagContainer.h"
#include "AbilityStateCheck_Base.generated.h"

/**
 * A base object for performing checks on the state of the player.
 * An instance will be created by the State Tag Handler.
 * 
 * To use this, create a child class and override the 'Run' function
 * to implement a specific condition/state check.
 * 
 * - If `Run` returns true, the specified gameplay tags will be added
 *   to the ability component as loose tags.
 * - If `Run` returns false, the specified gameplay tags will be removed.
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class GAS_TEST_API UAbilityStateCheck_Base : public UObject
{
	GENERATED_BODY()

	// Granting UAbilityStateTagHandler access to private members of this class
	friend class UAbilityStateTagHandler;

public:

	/** Overrides GetWorld to return the appropriate world context */
	virtual UWorld* GetWorld() const override;

	/** Checks if the object has been properly instantiated */
	bool IsInstantiated() const;

private:

	/** Gameplay tags that will be added if the condition check succeeds */
	UPROPERTY(EditDefaultsOnly, Meta = (ToolTip = "Tags to add if condition check is true"))
	FGameplayTagContainer TagsToAdd;

	/** Determines whether this state check should replicate */
	UPROPERTY(EditDefaultsOnly)
	bool bShouldReplicate = true;

protected:

	/**
	 * Blueprint-implementable event for performing a state check.
	 * 
	 * @param owner - The actor that owns this state check instance.
	 * @return true if the state check passes (tags will be added), false if it fails (tags will be removed).
	 */
	UFUNCTION(BlueprintImplementableEvent)
	bool Run(AActor* owner);
};
