#include "GameplayAbility_BaseTriggeredInputActionAbility.h"
#include "AbilityInputHandler.h"
#include "EnhancedInputComponent.h"
#include "GASInputConfig.h"

UGameplayAbility_BaseTriggeredInputActionAbility::UGameplayAbility_BaseTriggeredInputActionAbility(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer), bCancelAbilityOnInputReleased(true)
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;
}

void UGameplayAbility_BaseTriggeredInputActionAbility::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle, 
    const FGameplayAbilityActorInfo* ActorInfo, 
    const FGameplayAbilityActivationInfo ActivationInfo, 
    const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    bool bSuccess = false;

    // Get the player character and check if it's valid
    const APawn* PlayerCharacter = Cast<APawn>(GetAvatarActorFromActorInfo());
    if (!PlayerCharacter) 
    {
        return;
    }

    // Try to cast the input component once
    EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerCharacter->InputComponent);
    if (!EnhancedInputComponent)
    {
        return;  // Early return if input component is not found
    }

    // Get the ability input handler
    const UAbilityInputHandler* AbilityInputHandler = PlayerCharacter->FindComponentByClass<UAbilityInputHandler>();
    if (!AbilityInputHandler || !AbilityInputHandler->InputConfig) 
    {
        return;  // Early return if input config is invalid
    }

    // Get input mappings
    const TMap<UInputAction*, FEventActionPairTag>& InputAbilities = AbilityInputHandler->InputConfig->AbilityInputActions;

    // Iterate over input actions
    for (const TPair<UInputAction*, FEventActionPairTag>& ActionPair : InputAbilities)
    {
        UInputAction* InputAction = ActionPair.Key;
        const FEventActionPairTag& EventPairTag = ActionPair.Value;

        // Iterate over tagged actions
        for (const TPair<FGameplayTag, FEventActionPair>& TaggedActionPair : EventPairTag.TaggedAction)
        {
            FGameplayTag GameplayTag = TaggedActionPair.Key;
            const FEventActionPair& EventAction = TaggedActionPair.Value;

            // Iterate over event-action mappings
            for (const TPair<ETriggerEvent, GASInputEventType>& EventMapping : EventAction.EventAction)
            {
                ETriggerEvent TriggerEvent = EventMapping.Key;

                // Check if the gameplay tag matches the ability's tag
                if (GetAssetTags().HasTagExact(GameplayTag) && TriggerEvent == ETriggerEvent::Triggered)
                {
                    // Bind input action when triggered
                    const FEnhancedInputActionEventBinding& TriggeredEventBinding =
                        EnhancedInputComponent->BindAction(InputAction, ETriggerEvent::Triggered, this, 
                        &UGameplayAbility_BaseTriggeredInputActionAbility::OnTriggeredInputAction);

                    // Store event handle to track bindings
                    const uint32 TriggeredEventHandle = TriggeredEventBinding.GetHandle();
                    TriggeredEventHandles.AddUnique(TriggeredEventHandle);

                    UE_LOG(LogTemp, Log, TEXT("Bound Input: %s | Tag: %s | Trigger Event: Triggered"), 
                        *InputAction->GetName(), *GameplayTag.ToString());

                    bSuccess = true;
                }
            }
        }
    }

    // If successfully bound, commit ability
    if (bSuccess)
    {
        CommitAbility(Handle, ActorInfo, ActivationInfo);
    }
    else
    {
        constexpr bool bReplicateCancelAbility = true;
        CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
    }
}

void UGameplayAbility_BaseTriggeredInputActionAbility::EndAbility(
    const FGameplayAbilitySpecHandle Handle, 
    const FGameplayAbilityActorInfo* ActorInfo, 
    const FGameplayAbilityActivationInfo ActivationInfo, 
    bool bReplicateEndAbility, 
    bool bWasCancelled)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

    if (EnhancedInputComponent)
    {
        // Unbind the actions and reset input component
        for (const uint32 EventHandle : TriggeredEventHandles)
        {
            EnhancedInputComponent->RemoveBindingByHandle(EventHandle);
        }

        EnhancedInputComponent = nullptr;
    }

    TriggeredEventHandles.Reset();
}

void UGameplayAbility_BaseTriggeredInputActionAbility::InputReleased(
    const FGameplayAbilitySpecHandle Handle, 
    const FGameplayAbilityActorInfo* ActorInfo, 
    const FGameplayAbilityActivationInfo ActivationInfo)
{
    Super::InputReleased(Handle, ActorInfo, ActivationInfo);

    // If the ability should cancel on input released, do so
    if (bCancelAbilityOnInputReleased)
    {
        CancelAbility(Handle, ActorInfo, ActivationInfo, true);
    }
}