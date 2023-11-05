// Copyright Latar


#include "BorshGameplayTags.h"
#include "GameplayTagsManager.h"

// Declaring the Type
FBorshGameplayTags FBorshGameplayTags::GameplayTags;

// We need to call this function as soon as we can (Before any of those tags are attempted to be used)
void FBorshGameplayTags::InitializeNativeGameplayTags()
{
	// Get Gameplay Tag Manager (Manages the gameplay tags and we can also use it to add gameplay tags even natively here in C plus plus).
	GameplayTags.Attributes_Secondary_Armor = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.Armor"), FString("Reduces damage taken, improves block chance"));


}
