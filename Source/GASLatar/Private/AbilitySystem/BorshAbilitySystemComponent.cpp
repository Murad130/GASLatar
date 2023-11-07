// Copyright Latar


#include "AbilitySystem/BorshAbilitySystemComponent.h"
#include "BorshGameplayTags.h"

void UBorshAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UBorshAbilitySystemComponent::EffectApplied);

	// Using Gameplay Tags that we created in our singleton class
	const FBorshGameplayTags& GameplayTags = FBorshGameplayTags::Get();
	// Now we can access the tags
	// GameplayTags.Attributes_Secondary_Armor.ToString()
}

void UBorshAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	// if we want to show something to the screen that's in the view domain, it needs to receive something
	// from the middleman, the widget controller, which needs to receive something from the ability system component.
	// If that effect carries a tag, we can broadcast that to the widget controller, which can then broadcast it to the HUD

	// First we want to find out what gameplay tags the effect has
	// then get those tags and broadcast those in a delegate.

	// We don't store tags in an array but in an gameplay tag container
	FGameplayTagContainer TagContainer;

	// Getting tags that the effect has 
	EffectSpec.GetAllAssetTags(TagContainer);

	// Now we need to broadcast to our widget controller for every single tag that's in this container
	EffectAssetTags.Broadcast(TagContainer);


}
