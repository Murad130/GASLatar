// Copyright Latar

#include "AbilitySystem/Abilities/BorshFireBolt.h"
#include "GASLatar/Public/BorshGameplayTags.h"

FString UBorshFireBolt::GetDescription(int32 Level)
{
	const int32 Damage = GetDamageByDamageType(Level, FBorshGameplayTags::Get().Damage_Fire);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	if (Level == 1)
	{
		return FString::Printf(TEXT(
			// Title
			"<Title>FIRE BOLT</>\n\n"

			// Level
			"<Small>Level: </><Level>%d</>\n"
			// ManaCost
			"<Small>ManaCost: </><ManaCost>%.1f</>\n"
			// Cooldown
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

			// Descriptions
			"<Default>Launches a bolt of fire, "
			"exploding on impact and dealing: </>"

			// Damage
			"<Damage>%d</><Default> fire damage with"
			" a chance to burn</>"), 

			// Values
			Level,
			ManaCost,
			Cooldown,
			Damage);
	}
	else
	{
		return FString::Printf(TEXT(
			// Title
			"<Title>FIRE BOLT</>\n\n"

			// Level
			"<Small>Level: </><Level>%d</>\n"
			// ManaCost
			"<Small>ManaCost: </><ManaCost>%.1f</>\n"
			// Cooldown
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

			// Number of FireBolts
			"<Default>Launches %d bolts of fire, "
			"exploding on impact and dealing: </>"

			// Damage
			"<Damage>%d</><Default> fire damage with"
			" a chance to burn</>"),

			// Values
			Level,
			ManaCost,
			Cooldown,
			FMath::Min(Level, NumProjectiles),
			Damage);
	}
}

FString UBorshFireBolt::GetNextLevelDescription(int32 Level)
{
	const int32 Damage = GetDamageByDamageType(Level, FBorshGameplayTags::Get().Damage_Fire);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);

	return FString::Printf(TEXT(
		// Title
		"<Title>NEXT LEVEL: </>\n\n"

		// Level
		"<Small>Level: </><Level>%d</>\n"
		// ManaCost
		"<Small>ManaCost: </><ManaCost>%.1f</>\n"
		// Cooldown
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

		// Number of FireBolts
		"<Default>Launches %d bolts of fire, "
		"exploding on impact and dealing: </>"

		// Damage
		"<Damage>%d</><Default> fire damage with"
		" a chance to burn</>"),

		// Values
		Level,
		ManaCost,
		Cooldown,
		FMath::Min(Level, NumProjectiles),
		Damage);
	return FString::Printf(TEXT("<Title>NEXT LEVEL: </>\n\n<Default>Launches %d bolts of fire, exploding on impact and dealing: </><Damage>%d</><Default> fire damage with a chance to burn</>\n\n<Small>Level: </><Level>%d</>"), FMath::Min(Level, NumProjectiles), Damage, Level);
}