// Copyright Latar


#include "AbilitySystem/Data/CharacterClassInfo.h"

FCharacterClassDefaultInfo UCharacterClassInfo::GetClassDefaultInfo(ECharacterClass CharacterClass)
{
	// We're going to simply return and we'll perform a lookup in character class information.
	return CharacterClassInformation.FindChecked(CharacterClass);
}
