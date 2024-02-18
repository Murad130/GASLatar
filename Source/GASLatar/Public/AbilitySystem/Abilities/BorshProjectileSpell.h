// Copyright Latar

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/BorshDamageGameplayAbility.h"
#include "BorshProjectileSpell.generated.h"

class ABorshProjectile;
class UGameplayEffect;
struct FGameplayTag;

/**
 * 
 */
UCLASS()
class GASLATAR_API UBorshProjectileSpell : public UBorshDamageGameplayAbility
{
	GENERATED_BODY()
	

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SpawnProjectile(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag, bool bOverridePitch = false, float PitchOverride = 0.f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<ABorshProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly)
	int32 NumProjectiles = 5;

};
