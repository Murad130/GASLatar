// Copyright Latar


#include "AbilitySystem/BorshAbilitySystemGlobals.h"

#include "BorshAbilityTypes.h"

FGameplayEffectContext* UBorshAbilitySystemGlobals::AllocGameplayEffectContext() const
{
    return new FBorshGameplayEffectContext();
}
