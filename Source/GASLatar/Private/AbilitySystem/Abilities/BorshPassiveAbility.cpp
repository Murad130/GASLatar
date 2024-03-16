// Copyright Latar


#include "AbilitySystem/Abilities/BorshPassiveAbility.h"
#include "AbilitySystem/BorshAbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

void UBorshPassiveAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (UBorshAbilitySystemComponent* BorshASC = Cast<UBorshAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo())))
	{
		BorshASC->DeactivatePassiveAbility.AddUObject(this, &UBorshPassiveAbility::ReceiveDeactivate);
	}
}

void UBorshPassiveAbility::ReceiveDeactivate(const FGameplayTag& AbilityTag)
{
	if (AbilityTags.HasTagExact(AbilityTag))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}
