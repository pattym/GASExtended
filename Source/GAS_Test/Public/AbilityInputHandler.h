// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Components/ActorComponent.h"
#include "AbilityInputHandler.generated.h"


USTRUCT(BlueprintType)
struct FAbilityAssignerSpec
{
	GENERATED_BODY()

	//UPROPERTY(EditDefaultsOnly)
	//TSubclassOf<UGameplayAbility> Ability;
	
	UPROPERTY(EditDefaultsOnly)
	int Level = 0;

	UPROPERTY(EditDefaultsOnly)
	int inputID = -1;
	
	UPROPERTY(EditDefaultsOnly, Meta = (ToolTip = "Will try activate once added."))
	bool AutoActivate = false;

	UPROPERTY(EditDefaultsOnly, Meta = (EditCondition = "AutoActivate", ToolTip ="Once activated, will be removed when completed."))
	bool ActivateOnce = false;	
	
};

UCLASS()
class GAS_TEST_API UDefaultAbilities : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TMap<TSubclassOf<UGameplayAbility> ,FAbilityAssignerSpec> Abilities;

	
};


class UGASInputConfig;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAS_TEST_API UAbilityInputHandler : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAbilityInputHandler();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UGASInputConfig> InputConfig;
	
	void AbilityInput(FGameplayTag InInputTag);
	
	void EventInput(FGameplayTag InInputTag);
	
	void DynamicInput(FGameplayTag InInputTag);
	
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	UDefaultAbilities* DefaultAbilities = nullptr;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
private:
	void BindToInputConfig();

	UPROPERTY()
	UAbilitySystemComponent* AbilitySystem = nullptr;

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void BulkGiveAbilities(TMap<TSubclassOf<UGameplayAbility> ,FAbilityAssignerSpec> Abilities);

	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnControllerChanged(APawn* Pawn, AController* OldController, AController* NewController);

	void AddDefaultInputMappings(UGASInputConfig* InInputConfig, AController* Controller);
	void RemoveDefaultInputMappings(UGASInputConfig* InInputConfig, AController* Controller);

};
