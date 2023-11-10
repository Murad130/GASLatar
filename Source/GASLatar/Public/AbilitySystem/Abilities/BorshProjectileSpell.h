// Copyright Latar

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/BorshGameplayAbility.h"
#include "BorshProjectileSpell.generated.h"

class ABorshProjectile;

/**
 * 
 */
UCLASS()
class GASLATAR_API UBorshProjectileSpell : public UBorshGameplayAbility
{
	GENERATED_BODY()
	
protected:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<ABorshProjectile> ProjectileClass;

};
