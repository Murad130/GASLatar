// Copyright Latar

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 *  BorshGameplayTags
 * 
 * Singleton (Only one in the whole project and we set that at the project Level) containing native (Created within c++ & available in both c++ and Blueprint) Gameplay Tags
 * 
 */

struct FBorshGameplayTags
{

public:

	static const FBorshGameplayTags& Get() { return GameplayTags; }

	// We need a public function to initialize our tags. We want to be able to create those native Gameplay Tags
	static void InitializeNativeGameplayTags();

	/******************************************PRIMARY***************************************************/

	FGameplayTag Attributes_Primary_Strength;
	FGameplayTag Attributes_Primary_Intelligence;
	FGameplayTag Attributes_Primary_Resilience;
	FGameplayTag Attributes_Primary_Vigor;

	/******************************************SECONDARY*************************************************/

	FGameplayTag Attributes_Secondary_Armor;
	FGameplayTag Attributes_Secondary_ArmorPenetration;
	FGameplayTag Attributes_Secondary_BlockChance;
	FGameplayTag Attributes_Secondary_CriticalHitChance;
	FGameplayTag Attributes_Secondary_CriticalHitDamage;
	FGameplayTag Attributes_Secondary_CriticalHitResistance;
	FGameplayTag Attributes_Secondary_HealthRegeneration;
	FGameplayTag Attributes_Secondary_ManaRegeneration;
	FGameplayTag Attributes_Secondary_MaxHealth;
	FGameplayTag Attributes_Secondary_MaxMana;

protected:

private:
	// for a static variable like this, we need to go into the CPP file and explicitly declare the type
	static FBorshGameplayTags GameplayTags;


};
