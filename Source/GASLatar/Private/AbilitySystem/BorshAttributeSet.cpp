// Copyright Latar


#include "AbilitySystem/BorshAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "BorshAbilityTypes.h"
#include "GameFramework/Character.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "BorshGameplayTags.h"
#include "Interaction/CombatInterface.h"
#include "Interaction/PlayerInterface.h"
#include "Player/BorshPlayerController.h"
#include "AbilitySystem/BorshAbilitySystemLibrary.h"
#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"

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

	/***********************************************************RESISTANCES********************************************************************/

	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Arcane, GetArcaneResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Fire, GetFireResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Lightning, GetLightningResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Physical, GetPhysicalResistanceAttribute);
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

	/***********************************************************RESISTANCES********************************************************************/


	DOREPLIFETIME_CONDITION_NOTIFY(UBorshAttributeSet, FireResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBorshAttributeSet, LightningResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBorshAttributeSet, ArcaneResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBorshAttributeSet, PhysicalResistance, COND_None, REPNOTIFY_Always);

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

	if (Props.TargetCharacter->Implements<UCombatInterface>())
	{
		if (ICombatInterface::Execute_IsDead(Props.TargetCharacter))
		{
			return;
		}
	}

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// Setting the value to a clamped value(value from PreAttributeChange)
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		// Setting the value to a clamped value(value from PreAttributeChange)
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}
	// Checking to see if the evaluated data has the attribute incoming damage and then we can respond accordingly.
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		HandleIncomingDamage(Props);
	}
	if (Data.EvaluatedData.Attribute == GetIncomingXPAttribute())
	{
		HandleIncomingXP(Props);
	}
}

void UBorshAttributeSet::HandleIncomingDamage(const FEffectProperties& Props)
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
				FVector Impulse = UBorshAbilitySystemLibrary::GetDeathImpulse(Props.EffectContextHandle);
				CombatInterface->Die(UBorshAbilitySystemLibrary::GetDeathImpulse(Props.EffectContextHandle));
			}
			SendXPEvent(Props);
		}
		else
		{
			// We can activate the ability. but I'd like this Borsh attribute set to not depend on the enemy class. I don't want it to care what the owner of this is. We just want to activate an ability.
			// Well, here's how we can do it in a nice generic way. We can activate abilities by ability tag. activate an ability if you have an ability with a specific tag.
			FGameplayTagContainer TagContainer;
			TagContainer.AddTag(FBorshGameplayTags::Get().Effects_HitReact);
			Props.TargetASC->TryActivateAbilitiesByTag(TagContainer);

			const FVector& KnockbackForce = UBorshAbilitySystemLibrary::GetKnockbackForce(Props.EffectContextHandle);
			if (!KnockbackForce.IsNearlyZero(1.f))
			{
				Props.TargetCharacter->LaunchCharacter(KnockbackForce, true, true);
			}
		}


		const bool bBlock = UBorshAbilitySystemLibrary::IsBlockedHit(Props.EffectContextHandle);
		const bool bCriticalHit = UBorshAbilitySystemLibrary::IsCriticalHit(Props.EffectContextHandle);
		ShowFloatingText(Props, LocalIncomingDamage, bBlock, bCriticalHit);
		if (UBorshAbilitySystemLibrary::IsSuccessfulDebuff(Props.EffectContextHandle))
		{
			Debuff(Props);
		}
	}
}

void UBorshAttributeSet::Debuff(const FEffectProperties& Props)
{
	const FBorshGameplayTags& GameplayTags = FBorshGameplayTags::Get();
	FGameplayEffectContextHandle EffectContext = Props.SourceASC->MakeEffectContext();
	EffectContext.AddSourceObject(Props.SourceAvatarActor);

	const FGameplayTag DamageType = UBorshAbilitySystemLibrary::GetDamageType(Props.EffectContextHandle);
	const float DebuffDamage = UBorshAbilitySystemLibrary::GetDebuffDamage(Props.EffectContextHandle);
	const float DebuffDuration = UBorshAbilitySystemLibrary::GetDebuffDuration(Props.EffectContextHandle);
	const float DebuffFrequency = UBorshAbilitySystemLibrary::GetDebuffFrequency(Props.EffectContextHandle);

	FString DebuffName = FString::Printf(TEXT("DynamicDebuff_%s"), *DamageType.ToString());
	UGameplayEffect * Effect = NewObject<UGameplayEffect>(GetTransientPackage(), FName(DebuffName));

	Effect->DurationPolicy = EGameplayEffectDurationType::HasDuration;
	Effect->Period = DebuffFrequency;
	Effect->DurationMagnitude = FScalableFloat(DebuffDuration);

	// Grant Debuff Tag
	FInheritedTagContainer TagContainer = FInheritedTagContainer();
	UTargetTagsGameplayEffectComponent& Component = Effect->FindOrAddComponent<UTargetTagsGameplayEffectComponent>();
	TagContainer.Added.AddTag(GameplayTags.DamageTypesToDebuffs[DamageType]);
	TagContainer.CombinedTags.AddTag(GameplayTags.DamageTypesToDebuffs[DamageType]);
	Component.SetAndApplyTargetTagChanges(TagContainer);

	Effect->StackingType = EGameplayEffectStackingType::AggregateBySource;
	Effect->StackLimitCount = 1;

	const int32 Index = Effect->Modifiers.Num();
	Effect->Modifiers.Add(FGameplayModifierInfo());
	FGameplayModifierInfo& ModifierInfo = Effect->Modifiers[Index];

	ModifierInfo.ModifierMagnitude = FScalableFloat(DebuffDamage);
	ModifierInfo.ModifierOp = EGameplayModOp::Additive;
	ModifierInfo.Attribute = UBorshAttributeSet::GetIncomingDamageAttribute();

	if (FGameplayEffectSpec* MutableSpec = new FGameplayEffectSpec(Effect, EffectContext, 1.f))
	{
		FBorshGameplayEffectContext* BorshContext = static_cast<FBorshGameplayEffectContext*>(MutableSpec->GetContext().Get());		
		TSharedPtr<FGameplayTag> DebuffDamageType = MakeShareable(new FGameplayTag(DamageType));
		BorshContext->SetDamageType(DebuffDamageType);

		Props.TargetASC->ApplyGameplayEffectSpecToSelf(*MutableSpec);
	}
}

void UBorshAttributeSet::HandleIncomingXP(const FEffectProperties& Props)
{
	const float LocalIncomingXP = GetIncomingXP();
	SetIncomingXP(0.f);

	// Source Character is the owner, since GA_ListenForEvents applies GE_EventBasedEffect, adding to IncomingXP
	if (Props.SourceCharacter->Implements<UPlayerInterface>() && Props.SourceCharacter->Implements<UCombatInterface>())
	{
		// Should we level UP? if yes LevelUp
		const int32 CurrentLevel = ICombatInterface::Execute_GetPlayerLevel(Props.SourceCharacter);
		const int32 CurrentXP = IPlayerInterface::Execute_GetXP(Props.SourceCharacter);

		const int32 NewLevel = IPlayerInterface::Execute_FindLevelForXP(Props.SourceCharacter, CurrentXP + LocalIncomingXP);
		const int32 NumOfLevelUps = NewLevel - CurrentLevel;
		if (NumOfLevelUps > 0)
		{
			// TODO : Get AttributesPointsReward and SpellPointsReward
			const int32 AttributePointsReward = IPlayerInterface::Execute_GetAttributesPointsReward(Props.SourceCharacter, CurrentLevel);
			const int32 SpellPointsReward = IPlayerInterface::Execute_GetSpellPointsReward(Props.SourceCharacter, CurrentLevel);
			IPlayerInterface::Execute_AddToPlayerLevel(Props.SourceCharacter, NumOfLevelUps);
			IPlayerInterface::Execute_AddToAttributePoints(Props.SourceCharacter, AttributePointsReward);
			IPlayerInterface::Execute_AddToSpellPoints(Props.SourceCharacter, SpellPointsReward);

			// Fill up Health and Mana
			bTopOffHealth = true;
			bTopOffMana = true;

			IPlayerInterface::Execute_LevelUp(Props.SourceCharacter);
		}

		IPlayerInterface::Execute_AddToXP(Props.SourceCharacter, LocalIncomingXP);
	}
}

void UBorshAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetMaxHealthAttribute() && bTopOffHealth)
	{
		SetHealth(GetMaxHealth());
		bTopOffHealth = false;
	}
	if (Attribute == GetMaxManaAttribute() && bTopOffMana)
	{
		SetMana(GetMaxMana());
		bTopOffMana = false;
	}
}

void UBorshAttributeSet::ShowFloatingText(const FEffectProperties& Props, float Damage, bool bBlockedHit, bool bCriticalHit) const
{
	// So from the attribute set, we're going to get the local player controller that actually has a human player and call this function on it so we can see that damage number.
	// But we're going to call this function passing in the target character or actor that we want to attach or show this number above.
	if (Props.SourceCharacter != Props.TargetCharacter)
	{
		if (ABorshPlayerController* PC = Cast<ABorshPlayerController>(Props.SourceCharacter->Controller))
		{
			PC->ShowDamageNumber(Damage, Props.TargetCharacter, bBlockedHit, bCriticalHit);
			return;
		}
		// Showing floating text of damage when enemy hits our character
		if (ABorshPlayerController* PC = Cast<ABorshPlayerController>(Props.TargetCharacter->Controller))
		{
			PC->ShowDamageNumber(Damage, Props.TargetCharacter, bBlockedHit, bCriticalHit);
		}
	}
}

void UBorshAttributeSet::SendXPEvent(const FEffectProperties& Props)
{
	if (Props.TargetCharacter->Implements<UCombatInterface>())
	{
		const int32 TargetLevel = ICombatInterface::Execute_GetPlayerLevel(Props.TargetCharacter);
		const ECharacterClass TargetClass = ICombatInterface::Execute_GetCharacterClass(Props.TargetCharacter);
		const int32 XPReward = UBorshAbilitySystemLibrary::GetXPRewardForClassAndLevel(Props.TargetCharacter, TargetClass, TargetLevel);

		const FBorshGameplayTags& GameplayTags = FBorshGameplayTags::Get();
		FGameplayEventData Payload;
		Payload.EventTag = GameplayTags.Attributes_Meta_IncomingXP;
		Payload.EventMagnitude = XPReward;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Props.SourceCharacter, GameplayTags.Attributes_Meta_IncomingXP, Payload);
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

/***********************************************************RESISTANCES********************************************************************/

void UBorshAttributeSet::OnRep_FireResistance(const FGameplayAttributeData& OldFireResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBorshAttributeSet, FireResistance, OldFireResistance);
}

void UBorshAttributeSet::OnRep_LightningResistance(const FGameplayAttributeData& OldLightningResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBorshAttributeSet, LightningResistance, OldLightningResistance);
}

void UBorshAttributeSet::OnRep_ArcaneResistance(const FGameplayAttributeData& OldArcaneResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBorshAttributeSet, ArcaneResistance, OldArcaneResistance);
}

void UBorshAttributeSet::OnRep_PhysicalResistance(const FGameplayAttributeData& OldPhysicalResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBorshAttributeSet, PhysicalResistance, OldPhysicalResistance);
}



