// Copyright Latar


#include "AbilitySystem/BorshAttributeSet.h"
#include <Net/UnrealNetwork.h>

UBorshAttributeSet::UBorshAttributeSet()
{
	InitHealth(50.f);
	InitMaxHealth(100.f);
	InitMana(50.f);
	InitMaxMana(100.f);
}

void UBorshAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UBorshAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBorshAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
}

void UBorshAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBorshAttributeSet, Health, OldHealth);
}

void UBorshAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBorshAttributeSet, MaxHealth, OldMaxHealth);
}

void UBorshAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBorshAttributeSet, Mana, OldMana);
}

void UBorshAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBorshAttributeSet, MaxMana, OldMaxMana);
}