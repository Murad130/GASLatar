// Copyright Latar

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "BorshAbilitySystemGlobals.generated.h"

/**
 * 
 */
UCLASS()
class GASLATAR_API UBorshAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()

	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
	
};
