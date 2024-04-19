// Copyright Latar


#include "AbilitySystem/BorshAbilitySystemComponent.h"

#include "BorshGameplayTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/BorshAbilitySystemLibrary.h"
#include "AbilitySystem/Abilities/BorshGameplayAbility.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "BorshLogChannels.h"
#include "Interaction/PlayerInterface.h"

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
			AbilitySpec.DynamicAbilityTags.AddTag(FBorshGameplayTags::Get().Abilities_Status_Equipped);
			// And once we have a spec, we can simply give the ability with a function that exists on the ability
			GiveAbility(AbilitySpec);
			// DynamicAbilityTags are designed to be added and removed at runtime so we can change this later is we want to
		}
	}
	bStartupAbilitiesGiven = true;
	AbilitiesGivenDelegate.Broadcast();
}

void UBorshAbilitySystemComponent::AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupPassiveAbilities)
	{
		// So how do we grant the abilities ? we need to create an ability spec from each of these classes
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		AbilitySpec.DynamicAbilityTags.AddTag(FBorshGameplayTags::Get().Abilities_Status_Equipped);
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
	FScopedAbilityListLock ActiveScopeLoc(*this);
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

	FScopedAbilityListLock ActiveScopeLoc(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())  // GetActivatableAbilities returns a array of gameplay ability specs(That can be activated).
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag) && AbilitySpec.IsActive())
		{
			AbilitySpecInputReleased(AbilitySpec);
			InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, AbilitySpec.Handle, AbilitySpec.ActivationInfo.GetActivationPredictionKey());
		}
	}


}

void UBorshAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	FScopedAbilityListLock ActiveScopeLoc(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (AbilitySpec.IsActive())
			{
				InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, AbilitySpec.Handle, AbilitySpec.ActivationInfo.GetActivationPredictionKey());
			}
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

FGameplayTag UBorshAbilitySystemComponent::GetStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (FGameplayTag StatusTag : AbilitySpec.DynamicAbilityTags)
	{
		if (StatusTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Status"))))
		{
			return StatusTag;
		}
	}
	return FGameplayTag();
}

FGameplayTag UBorshAbilitySystemComponent::GetStatusFromAbilityTag(const FGameplayTag& AbilityTag)
{
	if (const FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
	{
		return GetStatusFromSpec(*Spec);
	}
	return FGameplayTag();
}

FGameplayTag UBorshAbilitySystemComponent::GetSlotFromAbilityTag(const FGameplayTag& AbilityTag)
{
	if (const FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
	{
		return GetInputTagFromSpec(*Spec);
	}
	return FGameplayTag();
}

bool UBorshAbilitySystemComponent::SlotIsEmpty(const FGameplayTag& Slot)
{
	// We need to figure out whether or not a given input tag iss occupied with a gameplay ability. How do we know that.
	// Well, we're going to need to check all of the gameplay abilities in our activatable abilities.
	FScopedAbilityListLock ActiveScopeLoc(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		// I need to know if this particular ability spec has a slot, right?
		if (AbilityHasSlot(AbilitySpec, Slot))
		{
			return false;
		}
	}
	return true;
}

bool UBorshAbilitySystemComponent::AbilityHasSlot(const FGameplayAbilitySpec& Spec, const FGameplayTag& Slot)
{
	return Spec.DynamicAbilityTags.HasTagExact(Slot);
}

bool UBorshAbilitySystemComponent::AbilityHasAnySlot(const FGameplayAbilitySpec& Spec)
{
	// Returns true if it has any slot no matter what it is 
	return Spec.DynamicAbilityTags.HasTag(FGameplayTag::RequestGameplayTag(FName("InputTag")));
}

FGameplayAbilitySpec* UBorshAbilitySystemComponent::GetSpecWithSlot(const FGameplayTag& Slot)
{
	// And how are we going to return the correct spec? Well, we need to loop through the abilities and see which one has this spec.
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		// We need to check in each abilities ability tags and its dynamic ability tags, and we want to see if it has this given slot.
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(Slot))
		{
			return &AbilitySpec;
		}
	}
	return nullptr;
}

bool UBorshAbilitySystemComponent::IsPassiveAbility(const FGameplayAbilitySpec& Spec) const
{
	// Our Ability Type(passive or offensive) is in AbilityInfo so we need to get the AbilityInfo if we want to check an ability type.
	const UAbilityInfo* AbilityInfo = UBorshAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	const FGameplayTag AbilityTag = GetAbilityTagFromSpec(Spec);
	// Now what we need here is the ability info struct that we can get from ability info.
	const FBorshAbilityInfo& Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
	const FGameplayTag AbilityType = Info.AbilityType;
	return AbilityType.MatchesTagExact(FBorshGameplayTags::Get().Abilities_Type_Passive);
}

void UBorshAbilitySystemComponent::AssignSlotToAbility(FGameplayAbilitySpec& Spec, const FGameplayTag& Slot)
{
	// So assigned slot to ability is going to first of all remove its input tag if it has one, and then assign it a new slot.
	ClearSlot(&Spec);
	Spec.DynamicAbilityTags.AddTag(Slot);
}

FGameplayAbilitySpec* UBorshAbilitySystemComponent::GetSpecFromAbilityTag(const FGameplayTag& AbilityTag)
{
	FScopedAbilityListLock ActiveScopeLoc(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		for (FGameplayTag Tag : AbilitySpec.Ability.Get()->AbilityTags)
		{
			if (Tag.MatchesTag(AbilityTag))
			{
				return &AbilitySpec;
			}
		}
	}
	return nullptr;
}

void UBorshAbilitySystemComponent::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		if (IPlayerInterface::Execute_GetAttributePoints(GetAvatarActor()) > 0)
		{
			ServerUpgradeAttribute(AttributeTag);
		}
	}
}

void UBorshAbilitySystemComponent::ServerUpgradeAttribute_Implementation(const FGameplayTag& AttributeTag)
{
	FGameplayEventData Payload;
	Payload.EventTag = AttributeTag;
	Payload.EventMagnitude = 1.f;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AttributeTag, Payload);

	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		IPlayerInterface::Execute_AddToAttributePoints(GetAvatarActor(), -1);
	}
}

void UBorshAbilitySystemComponent::UpdateAbilityStatuses(int32 Level)
{
	UAbilityInfo* AbilityInfo = UBorshAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	for (const FBorshAbilityInfo& Info : AbilityInfo->AbilityInformation)
	{
		if (!Info.AbilityTag.IsValid()) continue;
		if (Level < Info.LevelRequirement) continue;
		if (GetSpecFromAbilityTag(Info.AbilityTag) == nullptr)
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Info.Ability, 1);
			AbilitySpec.DynamicAbilityTags.AddTag(FBorshGameplayTags::Get().Abilities_Status_Eligible);
			GiveAbility(AbilitySpec);
			MarkAbilitySpecDirty(AbilitySpec);
			ClientUpdateAbilityStatus(Info.AbilityTag, FBorshGameplayTags::Get().Abilities_Status_Eligible, 1);
		}
	}
}

void UBorshAbilitySystemComponent::ServerSpendSpellPoint_Implementation(const FGameplayTag& AbilityTag)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		if (GetAvatarActor()->Implements<UPlayerInterface>())
		{
			IPlayerInterface::Execute_AddToSpellPoints(GetAvatarActor(), -1);
		}

		const FBorshGameplayTags GameplayTags = FBorshGameplayTags::Get();
		FGameplayTag Status = GetStatusFromSpec(*AbilitySpec);
		if (Status.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))
		{
			AbilitySpec->DynamicAbilityTags.RemoveTag(GameplayTags.Abilities_Status_Eligible);
			AbilitySpec->DynamicAbilityTags.AddTag(GameplayTags.Abilities_Status_Unlocked);
			Status = GameplayTags.Abilities_Status_Unlocked;
		}
		else if (Status.MatchesTagExact(GameplayTags.Abilities_Status_Equipped) || Status.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
		{
			AbilitySpec->Level += 1;
		}
		ClientUpdateAbilityStatus(AbilityTag, Status, AbilitySpec->Level);
		MarkAbilitySpecDirty(*AbilitySpec);
	}
}

void UBorshAbilitySystemComponent::ServerEquipAbility_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& Slot)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		// Now the first thing we're doing is we're getting our gameplay tags.
		const FBorshGameplayTags& GameplayTags = FBorshGameplayTags::Get();
		// We're storing the previous slot, but this is actually the previous slot in a sense of what slot did 
		// this particular ability have before before we went in and changed things.
		const FGameplayTag& PrevSlot = GetInputTagFromSpec(*AbilitySpec);
		// Same as Slot
		const FGameplayTag& Status = GetStatusFromSpec(*AbilitySpec);

		const bool bStatusValid = Status == GameplayTags.Abilities_Status_Equipped || Status == GameplayTags.Abilities_Status_Unlocked;
		if (bStatusValid)
		{
			// Handle Activation/Deactivation for passive abilities

			if (!SlotIsEmpty(Slot)) // There is an Ability in this slot Already. Deactivate and clear its slot.
			{
				// We also need to know what ability is there so we can deactivate it, at least if it's a passive ability.
				// And if we want to deactivate the ability, it would help if we knew which one it was so we can identify it with the ability spec.
				// For now we know that the slot we're trying to occupy is already occupied.
				FGameplayAbilitySpec* SpecWithSlot = GetSpecWithSlot(Slot);
				// And this is the ability sitting in that slot that we want to push out. But it could be a nullptr so we are making sure that it is not
				if (SpecWithSlot)
				{
					// is that ability the same as this ability ? If so, we can return early.
					if (AbilityTag.MatchesTagExact(GetAbilityTagFromSpec(*SpecWithSlot)))
					{
						ClientEquipAbility(AbilityTag, GameplayTags.Abilities_Status_Equipped, Slot, PrevSlot);
						return;
					}

					if (IsPassiveAbility(*SpecWithSlot))
					{
						// If the spec with slot is PassiveAbility that means we need to deactivate it when pushing it out from slot 
						// So how are we gonna do it ? We made a delegate FDeactivatePassiveAbility. And our passive abilities all bind to this.
						// So we can broadcast the ability tag through this, and all of our passive abilities will respond to it and check that tag.
						DeactivatePassiveAbility.Broadcast(GetAbilityTagFromSpec(*SpecWithSlot));
						// So this is going to result in our passive ability with this tag ending itself.
					}
					// after we've deactivated the passive ability, whether or not it's passive, we still want to clear the slot so it doesn't have that input tag anymore.
					ClearSlot(SpecWithSlot);
				}
			}
			// This takes care of the case where slot is not empty. So what do we do when we've passed that point?
			// Now we need to put our new ability in that slot. This ability could be passive(if passive we need to activate it if it's not already active.)
			if (!AbilityHasAnySlot(*AbilitySpec)) // Ability doesn't yet have a slot (it's not active)
			{
				if (IsPassiveAbility(*AbilitySpec))
				{
					TryActivateAbility(AbilitySpec->Handle);
				}
			}
			// Now that takes care of activating it. But what about its input tag ? It now needs to have a tag. So we need a function to assign a slot to an ability.
			AssignSlotToAbility(*AbilitySpec, Slot);
			MarkAbilitySpecDirty(*AbilitySpec);
		}
		ClientEquipAbility(AbilityTag, GameplayTags.Abilities_Status_Equipped, Slot, PrevSlot);
	}
}

void UBorshAbilitySystemComponent::ClientEquipAbility_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot)
{
	AbilityEquipped.Broadcast(AbilityTag, Status, Slot, PreviousSlot);
}

bool UBorshAbilitySystemComponent::GetDescriptionsByAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription, FString& OutNextLevelDescription)
{
	if (const FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		if (UBorshGameplayAbility* BorshAbility = Cast<UBorshGameplayAbility>(AbilitySpec->Ability))
		{
			OutDescription = BorshAbility->GetDescription(AbilitySpec->Level);
			OutNextLevelDescription = BorshAbility->GetNextLevelDescription(AbilitySpec->Level + 1);
			return true;
		}
	}
	const UAbilityInfo* AbilityInfo = UBorshAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	if (!AbilityTag.IsValid() || AbilityTag.MatchesTagExact(FBorshGameplayTags::Get().Abilities_None))
	{
		OutDescription = FString();
	}
	else
	{
		OutDescription = UBorshGameplayAbility::GetLockedDescription(AbilityInfo->FindAbilityInfoForTag(AbilityTag).LevelRequirement);
	}
	OutNextLevelDescription = FString();
	return false;
}

void UBorshAbilitySystemComponent::ClearSlot(FGameplayAbilitySpec* Spec)
{
	const FGameplayTag Slot = GetInputTagFromSpec(*Spec);
	Spec->DynamicAbilityTags.RemoveTag(Slot);
}

void UBorshAbilitySystemComponent::ClearAbilitiesOfSlot(const FGameplayTag& Slot)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (AbilityHasSlot(&Spec, Slot))
		{
			ClearSlot(&Spec);
		}
	}
}

bool UBorshAbilitySystemComponent::AbilityHasSlot(FGameplayAbilitySpec* Spec, const FGameplayTag& Slot)
{
	for (FGameplayTag Tag : Spec->DynamicAbilityTags)
	{
		if (Tag.MatchesTagExact(Slot))
		{
			return true;
		}
	}
	return false;
}

void UBorshAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	if (!bStartupAbilitiesGiven)
	{
		bStartupAbilitiesGiven = true;
		AbilitiesGivenDelegate.Broadcast();
	}
}

void UBorshAbilitySystemComponent::ClientUpdateAbilityStatus_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 AbilityLevel)
{
	AbilityStatusChanged.Broadcast(AbilityTag, StatusTag, AbilityLevel);
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
