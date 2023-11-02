// Copyright Latar


#include "AbilitySystem/BorshAbilitySystemComponent.h"

void UBorshAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UBorshAbilitySystemComponent::EffectApplied);
}

void UBorshAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	// So now we have a callback, but we want to bind this to that delegate back in ability system component.
	GEngine->AddOnScreenDebugMessage(1, 8.f, FColor::Blue, FString("Effect Applied!"));
}
