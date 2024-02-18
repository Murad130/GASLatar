// Copyright Latar

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/BorshGameplayAbility.h"
#include "Interaction/CombatInterface.h"
#include "BorshDamageGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class GASLATAR_API UBorshDamageGameplayAbility : public UBorshGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* TargetActor);

protected:
	// For Damage Effect
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	// Map for damage types
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	TMap<FGameplayTag, FScalableFloat> DamageTypes;


	UFUNCTION(BlueprintPure)
	FTaggedMontage GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontages) const;
	
	float GetDamageByDamageType(float InLevel, const FGameplayTag& DamageType);
};
