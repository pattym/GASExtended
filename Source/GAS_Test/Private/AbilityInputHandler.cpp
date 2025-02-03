// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityInputHandler.h"
#include "GASInputComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"


UAbilityInputHandler::UAbilityInputHandler()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

void UAbilityInputHandler::BeginPlay()
{
	Super::BeginPlay();
	if (InputConfig)
	{
		BindToInputConfig();
		
		APawn* OwnerPawn = Cast<APawn>(GetOwner());
		OwnerPawn->ReceiveControllerChangedDelegate.AddDynamic(this, &UAbilityInputHandler::OnControllerChanged);

		if (OwnerPawn->Controller->IsPlayerController())
		{
			AddDefaultInputMappings(InputConfig,OwnerPawn->Controller);
		}
	}
	
	if (DefaultAbilities)
	{
		BulkGiveAbilities(DefaultAbilities->Abilities);
	}
}


void UAbilityInputHandler::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UAbilityInputHandler::BindToInputConfig()
{
	// Get the Owner of this Actor Component
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		UE_LOG(LogTemp, Warning, TEXT("Owner not found!"));
		return;
	}

	// Try to get the Input Component (Only works if the owner is a Pawn)
	if (APawn* OwnerPawn = Cast<APawn>(Owner))
	{
		AbilitySystem = OwnerPawn->FindComponentByClass<UAbilitySystemComponent>();
		
		UGASInputComponent* InputComp = CastChecked<UGASInputComponent>(OwnerPawn->InputComponent);
		if (InputComp || AbilitySystem)
		{
			UE_LOG(LogTemp, Log, TEXT("GAS Component Found!"));
			InputComp->BindAbilityActions(InputConfig, this, &UAbilityInputHandler::AbilityInput, &UAbilityInputHandler::EventInput, &UAbilityInputHandler::DynamicInput);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("GAS Component NOT found!"));
		}
	}
}

void UAbilityInputHandler::BulkGiveAbilities(TMap<TSubclassOf<UGameplayAbility>, FAbilityAssignerSpec> Abilities)
{
	for (const TPair<TSubclassOf<UGameplayAbility>, FAbilityAssignerSpec>& AbilityPair : Abilities)
	{
		TSubclassOf<UGameplayAbility> AbilityClass = AbilityPair.Key;   // Extract the ability class
		const FAbilityAssignerSpec& AbilitySpec = AbilityPair.Value;    // Extract the ability spec

		if (AbilityClass) // Ensure the class reference is valid
		{
			FGameplayAbilitySpec GameplayAbilitySpec(AbilityClass, AbilitySpec.Level, AbilitySpec.inputID);
		
			if (AbilitySpec.AutoActivate)
			{
				if (AbilitySpec.ActivateOnce)
				{
					AbilitySystem->GiveAbilityAndActivateOnce(GameplayAbilitySpec);
					UE_LOG(LogTemp, Log, TEXT("Gave and activated ability once: %s"), *AbilityClass->GetName());
				}
				else
				{
					AbilitySystem->GiveAbility(GameplayAbilitySpec);
					AbilitySystem->TryActivateAbilityByClass(AbilityClass);
					UE_LOG(LogTemp, Log, TEXT("Gave and activated ability: %s"), *AbilityClass->GetName());
				}
			}
			else
			{
				AbilitySystem->GiveAbility(GameplayAbilitySpec);
				UE_LOG(LogTemp, Log, TEXT("Gave ability: %s"), *AbilityClass->GetName());
			}
		}
	}
}

void UAbilityInputHandler::AddDefaultInputMappings(UGASInputConfig* InInputConfig, AController* Controller)
{
	if (!InInputConfig)
	{
		UE_LOG(LogTemp, Warning, TEXT("InInputConfig is null!"));
		return;
	}
	
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		// Get the Enhanced Input Subsystem
		if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = PlayerController->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			// Loop through the DefaultInputMapping TMap
			for (const TPair<UInputMappingContext*, FICMPayload>& MappingPair : InInputConfig->DefaultInputMapping)
			{
				UInputMappingContext* MappingContext = MappingPair.Key;
				const FICMPayload& Payload = MappingPair.Value;

				// Add input context with the appropriate priority
				InputSubsystem->AddMappingContext(MappingContext, Payload.Priority, Payload.ContextOptions);

				// Log the addition of the input mapping context with its priority
				UE_LOG(LogTemp, Log, TEXT("Added input mapping context: %s with priority %d"), *MappingContext->GetName(), Payload.Priority);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Enhanced Input Subsystem not found on the Player Controller!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Not a player controller!"));
	}
}

void UAbilityInputHandler::RemoveDefaultInputMappings(UGASInputConfig* InInputConfig, AController* Controller)
{
	if (!InInputConfig)
	{
		UE_LOG(LogTemp, Warning, TEXT("InInputConfig is null!"));
		return;
	}
	
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		// Get the Enhanced Input Subsystem
		if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = PlayerController->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			// Loop through the DefaultInputMapping TMap
			for (const TPair<UInputMappingContext*, FICMPayload>& MappingPair : InInputConfig->DefaultInputMapping)
			{
				UInputMappingContext* MappingContext = MappingPair.Key;
				const FICMPayload& Payload = MappingPair.Value;

				// Add input context with the appropriate priority
				InputSubsystem->RemoveMappingContext(MappingContext, Payload.ContextOptions);

				// Log the addition of the input mapping context with its priority
				UE_LOG(LogTemp, Log, TEXT("Removed input mapping context: %s"), *MappingContext->GetName());
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Enhanced Input Subsystem not found on the Player Controller!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Not a player controller!"));
	}
}

void UAbilityInputHandler::AbilityInput(FGameplayTag InInputTag)
{
	if (AbilitySystem)
	{
		FGameplayTagContainer InputTags;
		InputTags.AddTag(InInputTag);
		
		// Attempt to activate ability
		bool bActivated = AbilitySystem->TryActivateAbilitiesByTag(InputTags, false);
		
		//UE_LOG(LogTemp, Warning, TEXT("Trying ability with tag: %s | Activation Success: %s"), 
		//	*InInputTag.ToString(), 
		//	bActivated ? TEXT("True") : TEXT("False"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AbilityInput failed: AbilitySystem is nullptr!"));
	}
}

void UAbilityInputHandler::EventInput(FGameplayTag InInputTag)
{
	if (AbilitySystem)
	{
		FGameplayEventData Data;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwner(), InInputTag, Data);
		
		//UE_LOG(LogTemp, Warning, TEXT("Performing Gameplay Event with tag: %s"), 
		//	*InInputTag.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("EventInput failed: AbilitySystem is nullptr!"));
	}
}

void UAbilityInputHandler::DynamicInput(FGameplayTag InInputTag)
{
	if (AbilitySystem->HasMatchingGameplayTag(InInputTag))
	{
		EventInput(InInputTag);
	}
	else
	{
		AbilityInput(InInputTag);
	}
}

void UAbilityInputHandler::OnControllerChanged(APawn* Pawn, AController* OldController, AController* NewController)
{
	//Remove Input Context Mappings provided by the Input Config from the old controller.
	RemoveDefaultInputMappings(InputConfig,OldController);

	//Clear input binds on the input component and rebind to input config... Will this work?
	if (UEnhancedInputComponent* FoundInputComponent = Cast<UEnhancedInputComponent>(Pawn->InputComponent))
	{
		FoundInputComponent->ClearActionBindings();
		BindToInputConfig();
	}
	
	//Add input Context Mappings provided by the Input Config to the new controller.
	AddDefaultInputMappings(InputConfig, NewController);
	
}
