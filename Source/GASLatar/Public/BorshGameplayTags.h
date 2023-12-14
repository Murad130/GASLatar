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

	/******************************************RESISTANCES***********************************************/

	FGameplayTag Attributes_Resistance_Fire;
	FGameplayTag Attributes_Resistance_Lightning;
	FGameplayTag Attributes_Resistance_Arcane;
	FGameplayTag Attributes_Resistance_Physical;

	FGameplayTag InputTag_LMB;
	FGameplayTag InputTag_RMB;
	FGameplayTag InputTag_a;
	FGameplayTag InputTag_z;
	FGameplayTag InputTag_e;
	FGameplayTag InputTag_r;

	FGameplayTag Damage;
	FGameplayTag Damage_Fire;
	FGameplayTag Damage_Lightning;
	FGameplayTag Damage_Arcane;
	FGameplayTag Damage_Physical;

	FGameplayTag Abilities_Attack;
	FGameplayTag Abilities_Summon;

	FGameplayTag Abilities_Fire_FireBolt;

	FGameplayTag CombatSocket_Weapon;
	FGameplayTag CombatSocket_RightHand;
	FGameplayTag CombatSocket_LeftHand;
	FGameplayTag CombatSocket_Tail;

	FGameplayTag Montage_Attack_1;
	FGameplayTag Montage_Attack_2;
	FGameplayTag Montage_Attack_3;
	FGameplayTag Montage_Attack_4;

	TMap<FGameplayTag, FGameplayTag> DamageTypesToResistances;
	
	FGameplayTag Effects_HitReact;

private:
	// for a static variable like this, we need to go into the CPP file and explicitly declare the type
	static FBorshGameplayTags GameplayTags;


};
