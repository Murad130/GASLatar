// Copyright Latar


#include "Player/BorshPlayerState.h"
#include <AbilitySystem/BorshAbilitySystemComponent.h>

ABorshPlayerState::ABorshPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UBorshAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);


	AttributeSet = CreateDefaultSubobject<UAttributeSet>("AttributeSet");

	NetUpdateFrequency = 100.f;
}

UAbilitySystemComponent* ABorshPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

