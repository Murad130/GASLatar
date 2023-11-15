// Copyright Latar


#include "AbilitySystem/BorshAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "BorshGameplayTags.h"
#include "Interaction/CombatInterface.h"

UBorshAttributeSet::UBorshAttributeSet()
{
	const FBorshGameplayTags& GameplayTags = FBorshGameplayTags::Get();


	// Takes a gameplay tag for the key and a delegate for the value

	/****************************************************************PRIMARY******************************************************************/
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Strength, GetStrengthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Intelligence, GetIntelligenceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Resilience, GetResilienceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Vigor, GetVigorAttribute);

	/****************************************************************SECONDARY****************************************************************/
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_Armor, GetArmorAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_ArmorPenetration, GetArmorPenetrationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_BlockChance, GetBlockChanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitChance, GetCriticalHitChanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitDamage, GetCriticalHitDamageAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitResistance, GetCriticalHitResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_HealthRegeneration, GetHealthRegenerationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_ManaRegeneration, GetManaRegenerationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MaxHealth, GetMaxHealthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MaxMana, GetMaxManaAttribute);



}

void UBorshAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	/*********************************************Primary************************************************************/

	DOREPLIFETIME_CONDITION_NOTIFY(UBorshAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBorshAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBorshAttributeSet, Resilience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBorshAttributeSet, Vigor, COND_None, REPNOTIFY_Always);

	/*********************************************Secondary*********************************************************/

	DOREPLIFETIME_CONDITION_NOTIFY(UBorshAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBorshAttributeSet, ArmorPenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBorshAttributeSet, BlockChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBorshAttributeSet, CriticalHitChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBorshAttributeSet, CriticalHitDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBorshAttributeSet, CriticalHitResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBorshAttributeSet, HealthRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBorshAttributeSet, ManaRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBorshAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBorshAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);

	/*********************************************Vital*********************************************************/

	DOREPLIFETIME_CONDITION_NOTIFY(UBorshAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBorshAttributeSet, Mana, COND_None, REPNOTIFY_Always);
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
		// but REMEMBER all this does is clamp what's returned from querying the modifier (this is not a legit set of the health attribute)
		// We need to set in our PostGameplayEffectExecute function.
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	if (Attribute == GetManaAttribute())
	{
		// Same as for health but here it is for mana
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());

	}

}

void UBorshAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const
{
	// Source = causer of the effect, Target = target of the effect (owner of this AS)

	// Getting our Effect Context
	Props.EffectContextHandle = Data.EffectSpec.GetContext();
	// Getting our Ability System Component from the source of this gameplay effect
	Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();

	if (IsValid(Props.SourceASC) && Props.SourceASC->AbilityActorInfo.IsValid() && Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		// Getting source actor that owns Ability System Component and storing it 
		Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();
		// Getting our Player Controller
		Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();
		if (Props.SourceController == nullptr && Props.SourceAvatarActor != nullptr)
		{
			// if SourceController is a null pointer SourceAvatarActor is not
			// then we'll attempt to get the player controller from the pawn directly.

			if (const APawn* Pawn = Cast<APawn>(Props.SourceAvatarActor))
			{
				Props.SourceController = Pawn->GetController();
				// So what we're doing here is we're first getting it from the ability actor info, but if ability actor info
			// contains a null pointer for the source controller, well then we'll just try to get it from the actor
			// by casting to a pawn and getting the controller then. So it's kind of a optional second step there.
			}

		}
		// If Source Controller get the Source Character
		if (Props.SourceController)
		{
			// Casting the pawn possessed by the controller to a character
			Props.SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
		}

		// By far we got a lot of info for Source Actor now we need to get info for the Target Actor

		// Checking if these things are valid. If Valid we can get the target Controller
		if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
		{
			Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
			Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
			Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);
			Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);

		}
	}


}

//Executed after a Gameplay Effect changes an Attribute
void UBorshAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FEffectProperties Props;
	SetEffectProperties(Data, Props);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// Setting the value to a clamped value(value from PreAttributeChange)
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
		UE_LOG(LogTemp, Warning, TEXT("Changed Health on %s, Health: %f"), *Props.TargetAvatarActor->GetName(), GetHealth());
	}
	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		// Setting the value to a clamped value(value from PreAttributeChange)
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}
	// Checking to see if the evaluated data has the attribute incoming damage and then we can respond accordingly.
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		// Now incoming damage being a meta attribute should be used for its value and then reset or zeroed out.
		const float LocalIncomingDamage = GetIncomingDamage();
		// So we're caching that value locally and then we can set that meta attribute to zero
		SetIncomingDamage(0.f);
		// So now that we have local incoming damage, we can decide what to do with it and we should only really do anything if its value isn't zero.
		if (LocalIncomingDamage > 0.f)
		{
			const float NewHealth = GetHealth() - LocalIncomingDamage;
			// And we can make sure that that is not going to be a negative value.
			SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));
			// At this point, we can tell certain things about the damage that has been done. For example, if new health is now zero, then we know that this damage caused was fatal.
			const bool bFatal = NewHealth <= 0.f;
			if (bFatal)
			{
				ICombatInterface* CombatInterface = Cast<ICombatInterface>(Props.TargetAvatarActor);
				if (CombatInterface)
				{
					CombatInterface->Die();
				}
			
			}
			else
			{
				// We can activate the ability. but I'd like this aura attribute set to not depend on the enemy class. I don't want it to care what the owner of this is. We just want to activate an ability.
				// Well, here's how we can do it in a nice generic way. We can activate abilities by ability tag. activate an ability if you have an ability with a specific tag.
				FGameplayTagContainer TagConatiner;
				TagConatiner.AddTag(FBorshGameplayTags::Get().Effects_HitReact);
				Props.TargetASC->TryActivateAbilitiesByTag(TagConatiner);
			}
		}
	}
}

/*********************************************Vital*********************************************************/

void UBorshAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBorshAttributeSet, Health, OldHealth);
}

void UBorshAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBorshAttributeSet, Mana, OldMana);
}

/*********************************************Primary************************************************************/

void UBorshAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBorshAttributeSet, Strength, OldStrength);
}

void UBorshAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBorshAttributeSet, Intelligence, OldIntelligence);
}

void UBorshAttributeSet::OnRep_Resilience(const FGameplayAttributeData& OldResilience) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBorshAttributeSet, Resilience, OldResilience);
}

void UBorshAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldVigor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBorshAttributeSet, Vigor, OldVigor);
}

/*********************************************Secondary*********************************************************/

void UBorshAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBorshAttributeSet, Armor, OldArmor);
}

void UBorshAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBorshAttributeSet, ArmorPenetration, OldArmorPenetration);
}

void UBorshAttributeSet::OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBorshAttributeSet, BlockChance, OldBlockChance);
}

void UBorshAttributeSet::OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBorshAttributeSet, CriticalHitChance, OldCriticalHitChance);
}

void UBorshAttributeSet::OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBorshAttributeSet, CriticalHitDamage, OldCriticalHitDamage);
}

void UBorshAttributeSet::OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBorshAttributeSet, CriticalHitResistance, OldCriticalHitResistance);
}

void UBorshAttributeSet::OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBorshAttributeSet, ManaRegeneration, OldManaRegeneration);
}

void UBorshAttributeSet::OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBorshAttributeSet, HealthRegeneration, OldHealthRegeneration);
}

void UBorshAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBorshAttributeSet, MaxHealth, OldMaxHealth);
}

void UBorshAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBorshAttributeSet, MaxMana, OldMaxMana);
}


