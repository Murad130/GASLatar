// Copyright Latar

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/BorshGameplayAbility.h"
#include "BorshDamageGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class GASLATAR_API UBorshDamageGameplayAbility : public UBorshGameplayAbility
{
	GENERATED_BODY()

protected:

	// For Damage Effect
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	// Map for damage types
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	TMap<FGameplayTag, FScalableFloat> DamageTypes;
	
};
