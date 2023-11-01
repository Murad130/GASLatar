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
	DOREPLIFETIME_CONDITION_NOTIFY(UBorshAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBorshAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
}

// Clamping Attributes to prevent them going above MaxValue or belove MinValue
// This function is called before Attribute Changes (ex: If our Health == MaxHealth 
// and we pick up a potion, before adding Health this function will be called 
// and in this example no heath will be added bcs Health == MaxHealth
void UBorshAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		// We are making sure that our health does not get past MaxHealth(ex: when we pick up a potion, crystal etc)
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
		
	}
	if (Attribute == GetManaAttribute())
	{
		// Same as for health but here it is for mana
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
		
	}

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