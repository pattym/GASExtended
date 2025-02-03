// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "GASInputConfig.h"
#include "GASInputComponent.generated.h"


UCLASS()
class GAS_TEST_API UGASInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

	virtual void InitializeComponent() override;

public:
	template<class UserClass,  typename AbilityFuncType, typename EventFuncType, typename DynamicFuncType>
	void BindAbilityActions(const UGASInputConfig* InputConfig, UserClass* Object, AbilityFuncType AbilityFunc, EventFuncType EventFunc, DynamicFuncType DynamicFunc);
};

template <class UserClass, typename AbilityFuncType, typename EventFuncType, typename DynamicFuncType>
void UGASInputComponent::BindAbilityActions(const UGASInputConfig* InputConfig, UserClass* Object,
	AbilityFuncType AbilityFunc, EventFuncType EventFunc, DynamicFuncType DynamicFunc)
{
    check(InputConfig);
    
    if (!Object)
    {
        UE_LOG(LogTemp, Error, TEXT("Object is null, cannot bind actions!"));
        return;
    }

    const TMap<UInputAction*, FEventActionPairTag>& Actions = InputConfig->AbilityInputActions;

    for (const TPair<UInputAction*, FEventActionPairTag>& ActionPair : Actions)
    {
        UInputAction* InputAction = ActionPair.Key;
        const FEventActionPairTag& InnerMap = ActionPair.Value;

        if (!InputAction)
        {
            UE_LOG(LogTemp, Warning, TEXT("Skipping null InputAction!"));
            continue;
        }

        // Ensure AbilityTriggerMappings exists inside FEventActionPairTag
        for (const TPair<FGameplayTag, FEventActionPair>& GameplayTagPair : InnerMap.TaggedAction)
        {
            FGameplayTag GameplayTag = GameplayTagPair.Key;
            const FEventActionPair& EventMap = GameplayTagPair.Value;

            for (const TPair<ETriggerEvent, GASInputEventType>& EventPair : EventMap.EventAction)
            {
                ETriggerEvent TriggerEvent = EventPair.Key;
                GASInputEventType EventType = EventPair.Value;
                
                // Ensure valid mapping
                if (!GameplayTag.IsValid())
                {
                    UE_LOG(LogTemp, Warning, TEXT("Skipping invalid GameplayTag for InputAction: %s"), *InputAction->GetName());
                    continue;
                }

                // Bind based on event type
                switch (EventType)
                {
                case GASInputEventType::GameplayAbility:
                    BindAction(InputAction, TriggerEvent, Object, AbilityFunc, GameplayTag);
                    UE_LOG(LogTemp, Log, TEXT("Bound as Ability: %s | Trigger: %s | Tag: %s"), 
                        *InputAction->GetName(), *UEnum::GetValueAsString(TriggerEvent), *GameplayTag.ToString());
                    break;

                case GASInputEventType::GameplayEvent:
                    BindAction(InputAction, TriggerEvent, Object, EventFunc, GameplayTag);
                    UE_LOG(LogTemp, Log, TEXT("Bound as Event: %s | Trigger: %s | Tag: %s"), 
                        *InputAction->GetName(), *UEnum::GetValueAsString(TriggerEvent), *GameplayTag.ToString());
                    break;
                    
                case GASInputEventType::GameplayDynamic:
                    BindAction(InputAction, TriggerEvent, Object, DynamicFunc, GameplayTag);
                    UE_LOG(LogTemp, Log, TEXT("Bound as Dynamic: %s | Trigger: %s | Tag: %s"), 
                        *InputAction->GetName(), *UEnum::GetValueAsString(TriggerEvent), *GameplayTag.ToString());
                    break;
                	
                case GASInputEventType::NotApplicable:
                	UE_LOG(LogTemp, Warning, TEXT("Input event not applicable. Skipping Bind!"));
                	break;

                default:
                    UE_LOG(LogTemp, Warning, TEXT("Unknown GASInputEventType for InputAction: %s"), *InputAction->GetName());
                    break;
                }
            }
        }
    }
}


	
	/*
	for (const FGASInputAction& Action : InputConfig->AbilityActions)
	{
		if (Action.InputAction)
		{
			TArray<ETriggerEvent> TriggerEvents;
			Action.TriggerTags.GetKeys(TriggerEvents);

			for (ETriggerEvent TriggerEvent : TriggerEvents)
			{
				GASInputEventType EventType = Action.TriggerTags.Find(TriggerEvent)->EventType;
				FGameplayTag FoundTag = Action.TriggerTags.Find(TriggerEvent)->InputTag;
				if (FoundTag.IsValid())
				{
					switch (EventType)
					{
					case GASInputEventType::GameplayAbility:
						BindAction(Action.InputAction, TriggerEvent, Object, AbilityFunc, FoundTag);
						UE_LOG(LogTemp, Warning, TEXT("Bound as ability: %s | Trigger Event: %s"), 
							*Action.InputAction->GetFName().ToString(), 
							*UEnum::GetValueAsString(TriggerEvent));
						break;

					case GASInputEventType::GameplayEvent:
						BindAction(Action.InputAction, TriggerEvent, Object, EventFunc, FoundTag);
						UE_LOG(LogTemp, Warning, TEXT("Bound as Event: %s | Trigger Event: %s"), 
							*Action.InputAction->GetFName().ToString(), 
							*UEnum::GetValueAsString(TriggerEvent));
						break;
					
					case GASInputEventType::GameplayDynamic:
						BindAction(Action.InputAction, TriggerEvent, Object, DynamicFunc, FoundTag);
						UE_LOG(LogTemp, Warning, TEXT("Bound as Dynamic: %s | Trigger Event: %s"), 
							*Action.InputAction->GetFName().ToString(), 
							*UEnum::GetValueAsString(TriggerEvent));
						break;
					}
				}
			}
		}
	}*/




/*
template <class UserClass, typename PressedFuncType, typename ReleaseFuncType, typename HeldFuncType>
void UGASInputComponent::BindAbilityActions(const UGASInputConfig* InputConfig, UserClass* Object,
	PressedFuncType PressedFunc, ReleaseFuncType ReleasedFunc, HeldFuncType HeldFunc)
{    // ** Object is where the Function lives ?? , FuncType is the function pointer/signature ** //
	check(InputConfig);
	// ** Bind the Press, release, held of the Inputs to GameplayTag** //
	for (const FGASInputAction& Action : InputConfig->AbilityActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{    // ** Bind every Action exist in Input Config, Tag will be passed to the Function(pointer/signature?) ** //
				BindAction(Action.InputAction, ETriggerEvent::Started, Object, PressedFunc, Action.InputTag);
			}
			if (ReleasedFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag);
			}
			if (HeldFunc)
			{    
				BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, HeldFunc, Action.InputTag);
			}
			
		}

		
	}
}*/

