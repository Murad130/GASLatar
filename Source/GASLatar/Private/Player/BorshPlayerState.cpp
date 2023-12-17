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
	DOREPLIFETIME(ABorshPlayerState, XP);
}

UAbilitySystemComponent* ABorshPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ABorshPlayerState::AddToXP(int32 InXP)
{
	XP += InXP;
	OnXpChangedDelegate.Broadcast(XP);
}

void ABorshPlayerState::AddToLevel(int32 InLevel)
{
	Level += InLevel;
	OnLevelChangedDelegate.Broadcast(Level);
}

void ABorshPlayerState::SetXP(int32 InXP)
{
	XP = InXP;
	OnXpChangedDelegate.Broadcast(XP);
}

void ABorshPlayerState::SetLevel(int32 InLevel)
{
	Level = InLevel;
	OnLevelChangedDelegate.Broadcast(Level);
}

void ABorshPlayerState::OnRep_Level(int32 OldLevel)
{
	OnLevelChangedDelegate.Broadcast(Level);
}

void ABorshPlayerState::OnRep_XP(int32 OldXP)
{
	OnXpChangedDelegate.Broadcast(XP);
}
