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

	// Variable to access armor 
	FGameplayTag Attributes_Secondary_Armor;

protected:

private:
	// for a static variable like this, we need to go into the CPP file and explicitly declare the type
	static FBorshGameplayTags GameplayTags;


};
