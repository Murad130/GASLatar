// Copyright Latar


#include "AbilitySystem/BorshAbilitySystemLibrary.h"

#include "BorshAbilityTypes.h"
#include "Game/BorshGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "UI/WidgetController/BorshWidgetController.h"
#include "UI/HUD/BorshHUD.h"
#include <Player/BorshPlayerState.h>

UOverlayWidgetController* UBorshAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	// Here we need to somehow, someway get the widget controller and return it to the caller. 
	// So how do we do that? Well, we need the player controller. So if we're calling this from within a widget, we're calling it from the perspective of a local player,
	// A widget will only exist for the local player and we'll want the player controller associated with that
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (ABorshHUD* BorshHUD = Cast<ABorshHUD>(PC->GetHUD()))
		{
			// now we have the HUD we want to call get overlay widget controller on the HUD.
			// But this getter requires 4 params (right now we have 1 param->PlayerController so first we need those variables
			ABorshPlayerState* PS = PC->GetPlayerState<ABorshPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			// So now we have the four key variables.
			const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
			// All we need to do now is get our HUD and call get overlay widget controller passing in widget controller params and this function needs to return that overlay widget controller.
			return BorshHUD->GetOverlayWidgetController(WidgetControllerParams);
		}
	}
	return nullptr;
}

UAttributeMenuWidgetController* UBorshAbilitySystemLibrary::GetAttributeMenuWidgetController(const UObject* WorldContextObject)
{
	// Here we need to somehow, someway get the widget controller and return it to the caller. 
	// So how do we do that? Well, we need the player controller. So if we're calling this from within a widget, we're calling it from the perspective of a local player,
	// A widget will only exist for the local player and we'll want the player controller associated with that
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (ABorshHUD* BorshHUD = Cast<ABorshHUD>(PC->GetHUD()))
		{
			// now we have the HUD we want to call get overlay widget controller on the HUD.
			// But this getter requires 4 params (right now we have 1 param->PlayerController so first we need those variables
			ABorshPlayerState* PS = PC->GetPlayerState<ABorshPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			// So now we have the four key variables.
			const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
			// All we need to do now is get our HUD and call get overlay widget controller passing in widget controller params and this function needs to return that overlay widget controller.
			return BorshHUD->GetAttributeMenuWidgetController(WidgetControllerParams);
		}
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

void UBorshAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	for (TSubclassOf<UGameplayAbility> AbilityClass : CharacterClassInfo->CommonAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		ASC->GiveAbility(AbilitySpec);
	}
}

UCharacterClassInfo* UBorshAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	ABorshGameModeBase* BorshGameMode = Cast<ABorshGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (BorshGameMode == nullptr) return nullptr;
	return BorshGameMode->CharacterClassInfo;
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

void UBorshAbilitySystemLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit)
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
