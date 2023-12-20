// Copyright Latar


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/BorshAttributeSet.h"
#include "AbilitySystem/BorshAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Player/BorshPlayerState.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	OnHealthChanged.Broadcast(GetBorshAS()->GetHealth());
	OnMaxHealthChanged.Broadcast(GetBorshAS()->GetMaxHealth());
	OnManaChanged.Broadcast(GetBorshAS()->GetMana());
	OnMaxManaChanged.Broadcast(GetBorshAS()->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	GetBorshPS()->OnXpChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnXPChanged);
	GetBorshPS()->OnLevelChangedDelegate.AddLambda(
		[this](int32 NewLevel)
		{
			OnPlayerLevelChangedDelegate.Broadcast(NewLevel);
		}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetBorshAS()->GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnHealthChanged.Broadcast(Data.NewValue);
		}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetBorshAS()->GetMaxHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnMaxHealthChanged.Broadcast(Data.NewValue);
		}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetBorshAS()->GetManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnManaChanged.Broadcast(Data.NewValue);
		}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetBorshAS()->GetMaxManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnMaxManaChanged.Broadcast(Data.NewValue);
		}
	);

	if (GetBorshASC())
	{
		// Here we check if StartupAbilities have been granted if not we give them to player
		if (GetBorshASC()->bStartupAbilitiesGiven)
		{
			BroadcastAbilityInfo();
		}
		else
		{
			GetBorshASC()->AbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::BroadcastAbilityInfo);
		}

		// this function makes things simplier for us bcs we don't have to go and declare callback functions for all the delegates that we want to bind to.
		GetBorshASC()->EffectAssetTags.AddLambda/*Anon function (doesn't have a name, not declared and not a member function */(
			[this](const FGameplayTagContainer& AssetTags)
			{

				// if we want to access a member variable that variable must be captured within lambda (in [] by adding 'this')

				// We put in the body here what do we want to happen when the assets tags container gets called
				// here we want to loop through the tag container and print it all to the screen.
				for (const FGameplayTag& Tag : AssetTags)
				{

					// For example, say that Tag = Message.HealthPotion

					// Matches Tag Determine if this tag matches TagToCheck 
					// "Message.HealthPotion".MatchesTag("Message") will return true, "Message".MatchesTag("Message.HealthPotion") will return false

					// In this case we don't care what message as long as it is a message tag and that's exactly the behavior we are looking for
					// If it's any other tag, say an ability tag, an input tag, any other kind of tag we define.
					// We don't want to look for a widget row in our message widget data table. We only want to look for a row in the table if the tag is a "Message" tag specifically.
					FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
					if (Tag.MatchesTag(MessageTag))
					{
						const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
						MessageWidgetRowDelegate.Broadcast(*Row);

						// Okay, so now we have a broadcast and all we need to do is bind to this delegate in our BP_Overlay class. 
					}
				}
			}
		);
	}
}

void UOverlayWidgetController::OnXPChanged(int32 NewXP)
{
	const ULevelUpInfo* LevelUpInfo = GetBorshPS()->LevelUpInfo;
	checkf(LevelUpInfo, TEXT("Unable to find LevelUpInfo. Please fill out BorshPlayerState Blueprint"));

	const int32 Level = LevelUpInfo->FindLevelForXP(NewXP);
	const int32 MaxLevel = LevelUpInfo->LevelUpInformation.Num();

	if (Level <= MaxLevel && Level > 0)
	{
		const int32 LevelUpRequirement = LevelUpInfo->LevelUpInformation[Level].LevelUpRequirement;
		const int32 PreviousLevelUpRequirement = LevelUpInfo->LevelUpInformation[Level - 1].LevelUpRequirement;

		const int32 DeltaLevelUpRequirement = LevelUpRequirement - PreviousLevelUpRequirement;
		const int32 XPForThisLevel = NewXP - PreviousLevelUpRequirement;

		const float XPBarPercent = static_cast<float>(XPForThisLevel) / static_cast<float>(DeltaLevelUpRequirement);

		OnXPPercentChangedDelegate.Broadcast(XPBarPercent);
	}

}

 