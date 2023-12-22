// Copyright Latar


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AbilitySystem/BorshAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/BorshPlayerState.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	BroadcastAbilityInfo();
	SpellPointsChanged.Broadcast(GetBorshPS()->GetSpellPoints());
	
}

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	GetBorshASC()->AbilityStatusChanged.AddLambda([this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag)
	{
			if (AbilityInfo)
			{
				FBorshAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
				Info.StatusTag = StatusTag;
				AbilityInfoDelegate.Broadcast(Info);
			}
	});

	GetBorshPS()->OnSpellPointsChangedDelegate.AddLambda([this](int32 SpellPoints)
		{
			SpellPointsChanged.Broadcast(SpellPoints);
		});
}
