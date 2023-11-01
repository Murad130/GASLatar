// Copyright Latar


#include "AbilitySystem/BorshAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "GameplayEffectExtension.h"
#include <Net/UnrealNetwork.h>

UBorshAttributeSet::UBorshAttributeSet()
{
	InitHealth(50.f);
	InitMaxHealth(100.f);
	InitMana(50.f);
	InitMaxMana(100.f);
}

void UBorshAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UBorshAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBorshAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBorshAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBorshAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
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
}

void UBorshAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBorshAttributeSet, Health, OldHealth);
}

void UBorshAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBorshAttributeSet, MaxHealth, OldMaxHealth);
}

void UBorshAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBorshAttributeSet, Mana, OldMana);
}

void UBorshAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBorshAttributeSet, MaxMana, OldMaxMana);
}


