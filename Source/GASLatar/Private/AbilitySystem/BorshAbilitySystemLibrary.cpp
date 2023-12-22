// Copyright Latar


#include "AbilitySystem/BorshAbilitySystemLibrary.h"

#include "BorshAbilityTypes.h"
#include "Game/BorshGameModeBase.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "UI/WidgetController/BorshWidgetController.h"
#include "UI/HUD/BorshHUD.h"
#include <Player/BorshPlayerState.h>

bool UBorshAbilitySystemLibrary::MakeWidgetControllerParams(const UObject* WorldContextObject, FWidgetControllerParams& OutWCParams, ABorshHUD*& OutBorshHUD)
{
	// Here we need to somehow, someway get the widget controller and return it to the caller. 
	// So how do we do that? Well, we need the player controller. So if we're calling this from within a widget, we're calling it from the perspective of a local player,
	// A widget will only exist for the local player and we'll want the player controller associated with that
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		OutBorshHUD = Cast<ABorshHUD>(PC->GetHUD());
		if (OutBorshHUD)
		{
			// now we have the HUD we want to call get overlay widget controller on the HUD.
			// But this getter requires 4 params (right now we have 1 param->PlayerController so first we need those variables
			ABorshPlayerState* PS = PC->GetPlayerState<ABorshPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();

			OutWCParams.AttributeSet = AS;
			OutWCParams.AbilitySystemComponent = ASC;
			OutWCParams.PlayerState = PS;
			OutWCParams.PlayerController = PC;
			return true;
		}
	}
	return false;
}

UOverlayWidgetController* UBorshAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	ABorshHUD* BorshHUD = nullptr;
	if (MakeWidgetControllerParams(WorldContextObject, WCParams, BorshHUD)) // same as || const bool bSuccessfulParams = MakeWidgetControllerParams(WorldContextObject, WCParams, BorshHUD) and use bSuccessfulParams in if();
	{
		return BorshHUD->GetOverlayWidgetController(WCParams);
	}
	return nullptr;
}

UAttributeMenuWidgetController* UBorshAbilitySystemLibrary::GetAttributeMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	ABorshHUD* BorshHUD = nullptr;
	if (MakeWidgetControllerParams(WorldContextObject, WCParams, BorshHUD)) // same as || const bool bSuccessfulParams = MakeWidgetControllerParams(WorldContextObject, WCParams, BorshHUD) and use bSuccessfulParams in if();
	{
		return BorshHUD->GetAttributeMenuWidgetController(WCParams);
	}
	return nullptr;
}

USpellMenuWidgetController* UBorshAbilitySystemLibrary::GetSpellMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	ABorshHUD* BorshHUD = nullptr;
	if (MakeWidgetControllerParams(WorldContextObject, WCParams, BorshHUD)) // same as || const bool bSuccessfulParams = MakeWidgetControllerParams(WorldContextObject, WCParams, BorshHUD) and use bSuccessfulParams in if();
	{
		return BorshHUD->GetSpellMenuWidgetController(WCParams);
	}
	return nullptr;
}

void UBorshAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC)
{
	AActor* AvatarActor = ASC->GetAvatarActor();

	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	FCharacterClassDefaultInfo ClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);

	FGameplayEffectContextHandle PrimaryAttributesContextHandle = ASC->MakeEffectContext();
	PrimaryAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle PrimaryAttributesSpecHandle = ASC->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttributes, Level, PrimaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle SecondaryAttributesContextHandle = ASC->MakeEffectContext();
	SecondaryAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle SecondaryAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttributes, Level, SecondaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle VitalAttributesContextHandle = ASC->MakeEffectContext();
	VitalAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle VitalAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->VitalAttributes, Level, VitalAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalAttributesSpecHandle.Data.Get());
}

void UBorshAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr) return;
	for (TSubclassOf<UGameplayAbility> AbilityClass : CharacterClassInfo->CommonAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		ASC->GiveAbility(AbilitySpec);
	}
	const FCharacterClassDefaultInfo& DefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
	for (TSubclassOf<UGameplayAbility> AbilityClass : DefaultInfo.StartupAbilities)
	{
		// Get player level
		if (ASC->GetAvatarActor()->Implements<UCombatInterface>())
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, ICombatInterface::Execute_GetPlayerLevel(ASC->GetAvatarActor()));
			ASC->GiveAbility(AbilitySpec);
		}
	}
}

int32 UBorshAbilitySystemLibrary::GetXPRewardForClassAndLevel(const UObject* WorldContextObject, ECharacterClass CharacterClass, int32 CharacterLevel)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr) return 0;

	const FCharacterClassDefaultInfo& Info = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
	const float XPReward = Info.XPReward.GetValueAtLevel(CharacterLevel);

	return static_cast<int32>(XPReward);
}

UCharacterClassInfo* UBorshAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	const ABorshGameModeBase* BorshGameMode = Cast<ABorshGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (BorshGameMode == nullptr) return nullptr;
	return BorshGameMode->CharacterClassInfo;
}

UAbilityInfo* UBorshAbilitySystemLibrary::GetAbilityInfo(const UObject* WorldContextObject)
{
	const ABorshGameModeBase* BorshGameMode = Cast<ABorshGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (BorshGameMode == nullptr) return nullptr;
	return BorshGameMode->AbilityInfo;
}

bool UBorshAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FBorshGameplayEffectContext* BorshEffectContext = static_cast<const FBorshGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return BorshEffectContext->IsBlockedHit();
	}
	return false;
}

bool UBorshAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FBorshGameplayEffectContext* BorshEffectContext = static_cast<const FBorshGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return BorshEffectContext->IsCriticalHit();
	}
	return false;
}

void UBorshAbilitySystemLibrary::SetIsBlockedHit(UPARAM(ref)FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit)
{
	if (FBorshGameplayEffectContext* BorshEffectContext = static_cast<FBorshGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		BorshEffectContext->SetIsBlockedHit(bInIsBlockedHit);
	}
}

void UBorshAbilitySystemLibrary::SetIsCriticalHit(UPARAM(ref)FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit)
{
	if (FBorshGameplayEffectContext* BorshEffectContext = static_cast<FBorshGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		BorshEffectContext->SetIsCriticalHit(bInIsCriticalHit);
	}
}

void UBorshAbilitySystemLibrary::GetLivePlayersWithinRadius(const UObject* WorldContextObject, TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius, const FVector& SphereOrigin)
{
	// We need to perform a check for a sphere in the world and get all actors inside of that sphere
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore);

	TArray<FOverlapResult> Overlaps;
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		World->OverlapMultiByObjectType(Overlaps, SphereOrigin, FQuat::Identity, FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects), FCollisionShape::MakeSphere(Radius), SphereParams);
		for (FOverlapResult& Overlap : Overlaps)
		{
			if (Overlap.GetActor()->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(Overlap.GetActor()))
			{
				OutOverlappingActors.AddUnique(ICombatInterface::Execute_GetAvatar(Overlap.GetActor()));
			}
		}
	}
}

bool UBorshAbilitySystemLibrary::IsNotFriend(AActor* FirstActor, AActor* SecondActor)
{
	const bool bBothArePlayers = FirstActor->ActorHasTag(FName("Player")) && SecondActor->ActorHasTag(FName("Player"));
	const bool bBothAreEnemies = FirstActor->ActorHasTag(FName("Enemy")) && SecondActor->ActorHasTag(FName("Enemy"));
	const bool bFriends = bBothArePlayers || bBothAreEnemies;
	return !bFriends;
}


