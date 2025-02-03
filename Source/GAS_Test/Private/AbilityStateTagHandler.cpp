// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityStateTagHandler.h"
#include "AbilitySystemBlueprintLibrary.h"

UAbilityStateTagHandler::UAbilityStateTagHandler()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAbilityStateTagHandler::BeginPlay()
{
	Super::BeginPlay();

	// Early return if AbilityStateTag is null
	if (!AbilityStateTag)
	{
		UE_LOG(LogTemp, Error, TEXT("AbilityStateTag is null in %s!"), *GetName());
		return;
	}

	// Try to find the Ability System Component
	OwnersASC = GetOwner()->FindComponentByClass<UAbilitySystemComponent>();
	if (OwnersASC)
	{
		// Loop through each state check class and instantiate it
		for (TSubclassOf<UAbilityStateCheck_Base> StateCheckClass : AbilityStateTag->StateChecks)
		{
			if (StateCheckClass) // Ensure the class is valid before trying to create an instance
			{
				// Create an instance of the class dynamically
				if (UAbilityStateCheck_Base* NewStateCheck = NewObject<UAbilityStateCheck_Base>(this, StateCheckClass))
				{
					AbilityStateCheckInstances.Add(NewStateCheck); // Store the new instance
					UE_LOG(LogTemp, Log, TEXT("Created and added AbilityStateCheck instance: %s"), *StateCheckClass->GetName());
				}
			}
		}
	}
	else
	{
		// Log an error and disable tick if ASC isn't found
		SetComponentTickEnabled(false);
		UE_LOG(LogTemp, Error, TEXT("No AbilitySystemComponent found on owner, disabling tick!"));
	}
}

void UAbilityStateTagHandler::TickComponent(float DeltaTime, ELevelTick TickType,
                                            FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Early return if Ability System Component is not found
	if (!OwnersASC)
	{
		UE_LOG(LogTemp, Error, TEXT("No Ability System Component on owner, skipping tick."));
		return;
	}

	// Loop through each AbilityStateCheck instance
	for (UAbilityStateCheck_Base* StateCheckInstance : AbilityStateCheckInstances)
	{
		if (StateCheckInstance) // Ensure the instance is valid
		{
			// Perform logic on the state check instance
			if (StateCheckInstance->TagsToAdd.IsEmpty())
			{
				UE_LOG(LogTemp, Log, TEXT("No gameplay tags specified in state check object: %s"), *StateCheckInstance->GetName());
			}
			else
			{
				// Cache the tags container to optimize calls
				const FGameplayTagContainer& TagsToAdd = StateCheckInstance->TagsToAdd;

				// Determine whether to replicate
				bool ShouldReplicate = StateCheckInstance->bShouldReplicate;

				// Run the check and update the tags accordingly
				if (StateCheckInstance->Run(GetOwner()))
				{
					// Add missing tags if the condition is true
					UAbilitySystemBlueprintLibrary::AddLooseGameplayTags(GetOwner(), GetMissingTags(TagsToAdd.GetGameplayTagArray()), ShouldReplicate);
				}
				else
				{
					// Remove the tags if the condition is false
					UAbilitySystemBlueprintLibrary::RemoveLooseGameplayTags(GetOwner(), TagsToAdd, ShouldReplicate);
				}
			}
		}
	}
}

FGameplayTagContainer UAbilityStateTagHandler::GetMissingTags(TArray<FGameplayTag> Tags)
{
	FGameplayTagContainer NeedToAddTags;

	// Loop through each tag and check if it exists on the AbilitySystemComponent
	for (const FGameplayTag& tag : Tags)
	{
		if (OwnersASC->GetGameplayTagCount(tag) == 0)
		{
			NeedToAddTags.AddTag(tag);
		}
	}
	return NeedToAddTags;
}
