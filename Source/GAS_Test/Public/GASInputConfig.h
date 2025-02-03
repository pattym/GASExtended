// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputSubsystemInterface.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "GASInputConfig.generated.h"

UENUM(BlueprintType) 
enum class GASInputEventType : uint8
{
	GameplayAbility UMETA(DisplayName = "Gameplay Ability"),
	GameplayEvent UMETA(DisplayName = "Gameplay Event"),
	GameplayDynamic UMETA(DisplayName = "Gameplay Dynamic"),
	NotApplicable UMETA(DisplayName = "Not Applicable")
};


USTRUCT(BlueprintType)
struct FEventActionPair
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TMap<ETriggerEvent,GASInputEventType> EventAction;
};

USTRUCT(BlueprintType)
struct FEventActionPairTag
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TMap<FGameplayTag,FEventActionPair> TaggedAction;
};

USTRUCT(BlueprintType)
struct FICMPayload
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	int Priority = 0;

	UPROPERTY(EditDefaultsOnly)
	FModifyContextOptions ContextOptions;
};


UCLASS()
class GAS_TEST_API UGASInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TMap<class UInputAction*, FEventActionPairTag> AbilityInputActions;

	UPROPERTY(EditDefaultsOnly)
	TMap<class UInputMappingContext*, FICMPayload> DefaultInputMapping;
};
