#pragma once

#include "CoreMinimal.h"
#include "AbilityStateCheck_Base.h"
#include "Components/ActorComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilityStateTagHandler.generated.h"

/**
 * Data asset that holds a set of state check classes.
 * This allows designers to specify a group of state check objects to be used in gameplay.
 */
UCLASS()
class GAS_TEST_API UAbilityStateCheckObjects : public UDataAsset
{
	GENERATED_BODY()

public:
	/** A set of state check classes that will be used to evaluate ability-related conditions. */
	UPROPERTY(EditDefaultsOnly)
	TSet<TSubclassOf<UAbilityStateCheck_Base>> StateChecks;
};

/**
 * An actor component responsible for managing ability state tags.
 * 
 * This component checks conditions through AbilityStateCheck instances and applies/removes gameplay tags accordingly.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GAS_TEST_API UAbilityStateTagHandler : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAbilityStateTagHandler();

	/** Reference to a data asset that holds state check classes. */
	UPROPERTY(EditDefaultsOnly)
	UAbilityStateCheckObjects* AbilityStateTag = nullptr;

protected:
	/** Called when the game starts or when the component is first initialized. */
	virtual void BeginPlay() override;

public:
	/** Called every frame to update the component. */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

private:
	/** Reference to the owner's Ability System Component, used for managing gameplay abilities and tags. */
	UPROPERTY()
	UAbilitySystemComponent* OwnersASC = nullptr;

	/** Array of instantiated state check objects that will be evaluated. */
	UPROPERTY()
	TArray<UAbilityStateCheck_Base*> AbilityStateCheckInstances;

	/** Determines which gameplay tags are missing from the provided list and need to be added. */
	FGameplayTagContainer GetMissingTags(TArray<FGameplayTag> Tags);
};

