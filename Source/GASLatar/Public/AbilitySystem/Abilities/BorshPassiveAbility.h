// Copyright Latar

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/BorshGameplayAbility.h"
#include "BorshPassiveAbility.generated.h"

/**
 * 
 */
UCLASS()
class GASLATAR_API UBorshPassiveAbility : public UBorshGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	void ReceiveDeactivate(const FGameplayTag& AbilityTag);

};
