// Copyright Latar


#include "UI/WidgetController/BorshWidgetController.h"

#include "Player/BorshPlayerController.h"
#include "Player/BorshPlayerState.h"
#include "AbilitySystem/BorshAbilitySystemComponent.h"
#include "AbilitySystem/BorshAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"

void UBorshWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}

void UBorshWidgetController::BroadcastInitialValues()
{

}

void UBorshWidgetController::BindCallbacksToDependencies()
{
}

void UBorshWidgetController::BroadcastAbilityInfo()
{
	// Get information about all given abilities, look up their Ability Info, and broadcast it to widgets
	if (!GetBorshASC()->bStartupAbilitiesGiven) return;

	FForEachAbility BroadcastDelegate;
	BroadcastDelegate.BindLambda([this](const FGameplayAbilitySpec& AbilitySpec)
		{
			FBorshAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(GetBorshASC()->GetAbilityTagFromSpec(AbilitySpec));
			Info.InputTag = GetBorshASC()->GetInputTagFromSpec(AbilitySpec);
			Info.StatusTag = BorshAbilitySystemComponent->GetStatusFromSpec(AbilitySpec);
			AbilityInfoDelegate.Broadcast(Info);
		});
	GetBorshASC()->ForEachAbility(BroadcastDelegate);
}

ABorshPlayerController* UBorshWidgetController::GetBorshPC()
{
	if (BorshPlayerController == nullptr)
	{
		BorshPlayerController = Cast<ABorshPlayerController>(PlayerController);
	}
	return BorshPlayerController;
}

ABorshPlayerState* UBorshWidgetController::GetBorshPS()
{
	if (BorshPlayerState == nullptr)
	{
		BorshPlayerState = Cast<ABorshPlayerState>(PlayerState);
	}
	return BorshPlayerState;
}

UBorshAbilitySystemComponent* UBorshWidgetController::GetBorshASC()
{
	if (BorshAbilitySystemComponent == nullptr)
	{
		BorshAbilitySystemComponent = Cast<UBorshAbilitySystemComponent>(AbilitySystemComponent);
	}
	return BorshAbilitySystemComponent;
}

UBorshAttributeSet* UBorshWidgetController::GetBorshAS()
{
	if (BorshAttributeSet == nullptr)
	{
		BorshAttributeSet = Cast<UBorshAttributeSet>(AttributeSet);
	}
	return BorshAttributeSet;
}
