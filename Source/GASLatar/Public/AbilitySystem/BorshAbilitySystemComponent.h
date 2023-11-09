// Copyright Latar

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "BorshAbilitySystemComponent.generated.h"

// delegate for broadcasting the asset tags.
DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /*AssetTags*/);

/**
 * 
 */
UCLASS()
class GASLATAR_API UBorshAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
	void AbilityActorInfoSet();

	FEffectAssetTags EffectAssetTags;

	// Function that we can call to give start up abilities.
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);
	// Function to Activate Abilities (Held and Released Abilities)
	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

protected:
	void EffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);

};
