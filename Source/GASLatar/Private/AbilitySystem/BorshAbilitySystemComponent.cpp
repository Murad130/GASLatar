// Copyright Latar


#include "AbilitySystem/BorshAbilitySystemComponent.h"

#include "BorshGameplayTags.h"
#include "AbilitySystem/Abilities/BorshGameplayAbility.h"
#include "BorshLogChannels.h"

void UBorshAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UBorshAbilitySystemComponent::ClientEffectApplied);

	// Using Gameplay Tags that we created in our singleton class
	const FBorshGameplayTags& GameplayTags = FBorshGameplayTags::Get();
	// Now we can access the tags
	// GameplayTags.Attributes_Secondary_Armor.ToString()
}

void UBorshAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
	{
		// So how do we grant the abilities ? we need to create an ability spec from each of these classes
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		if (const UBorshGameplayAbility* BorshAbility = Cast<UBorshGameplayAbility>(AbilitySpec.Ability))
		{
			// Now we need to give the player possibility to change the Input for Ability
			AbilitySpec.DynamicAbilityTags.AddTag(BorshAbility->StartupInputTag);
			// And once we have a spec, we can simply give the ability with a function that exists on the ability
			GiveAbility(AbilitySpec);
			// DynamicAbilityTags are designed to be added and removed at runtime so we can change this later is we want to
		}
	}
	bStartupAbilitiesGiven = true;
	AbilitiesGivenDelegate.Broadcast(this);
}

void UBorshAbilitySystemComponent::AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupPassiveAbilities)
	{
		// So how do we grant the abilities ? we need to create an ability spec from each of these classes
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		GiveAbilityAndActivateOnce(AbilitySpec);
	}
}

void UBorshAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	// Accessing our ability system component and call its ability input tag pressed.
	// Now we need to check if they're not already active (I don't want to activate them every single frame while we're holding input).
	// But first we need to make sure the input tag is valid
	if (!InputTag.IsValid()) return;

	// After that I want to check to see if this ability system component has any activatable abilities with this input tag.
	// So how do we do that? Well, we have a function that contains all of our activatable abilities.
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())  // GetActivatableAbilities returns a array of gameplay ability specs(That can be activated).
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			// To tell an ability that its input is being pressed.
			AbilitySpecInputPressed(AbilitySpec); // sets a boolean on the ability spec that is keeping track of whether or not its particular input is pressed.

			// So now we want to activate it if it's not already active. 
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);

			}
		}
	}

}

void UBorshAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	// We don't want to just end an ability if its input is released. We want the ability to determine that because not all abilities need to be canceled or ended when their ability is no longer pressed. 
	// We don't want to necessarily end that ability as soon as it's released.

	if (!InputTag.IsValid()) return;

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())  // GetActivatableAbilities returns a array of gameplay ability specs(That can be activated).
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputReleased(AbilitySpec); 
		}
	}


}

void UBorshAbilitySystemComponent::ForEachAbility(const FForEachAbility& Delegate)
{
	// we have to be careful because abilities can change status. They can become no longer activatable for example, maybe they're blocked by a certain gameplay tag,
	// So a good practice when looping over this container is to lock that activatable abilities container until this for loop is done and we can do that with F scoped ability list lock
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!Delegate.ExecuteIfBound(AbilitySpec))
		{
			UE_LOG(LogBorsh, Error, TEXT("Failed to execute delegate in %hs"), __FUNCTION__);
		}
	}
}

FGameplayTag UBorshAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability)
	{
		for (FGameplayTag Tag : AbilitySpec.Ability.Get()->AbilityTags)
		{
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities"))))
			{
				return Tag;
			}
		}
	}
	return FGameplayTag();
}

FGameplayTag UBorshAbilitySystemComponent::GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (FGameplayTag Tag : AbilitySpec.DynamicAbilityTags)
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag"))))
		{
			return Tag;
		}
	}
	return FGameplayTag();
}

void UBorshAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	if (!bStartupAbilitiesGiven)
	{
		bStartupAbilitiesGiven = true;
		AbilitiesGivenDelegate.Broadcast(this);
	}
}

void UBorshAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
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
