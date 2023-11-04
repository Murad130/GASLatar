// Copyright Latar


#include "Player/BorshPlayerState.h"
#include <AbilitySystem/BorshAbilitySystemComponent.h>
#include <AbilitySystem/BorshAttributeSet.h>
#include "Net/UnrealNetwork.h"

ABorshPlayerState::ABorshPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UBorshAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UBorshAttributeSet>("AttributeSet");

	NetUpdateFrequency = 100.f;
}

void ABorshPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABorshPlayerState, Level);
}

UAbilitySystemComponent* ABorshPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ABorshPlayerState::OnRep_Level(int32 OldLevel)
{

}
