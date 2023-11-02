// Copyright Latar


#include "UI/WidgetController/OverlayWidgetController.h"
#include <AbilitySystem/BorshAttributeSet.h>
#include <AbilitySystem/BorshAbilitySystemComponent.h>

void UOverlayWidgetController::BroadcastInitialValues()
{
	const UBorshAttributeSet* BorshAttributeSet = CastChecked<UBorshAttributeSet>(AttributeSet);

	OnHealthChanged.Broadcast(BorshAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(BorshAttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(BorshAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(BorshAttributeSet->GetMaxMana());
	
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	const UBorshAttributeSet* BorshAttributeSet = CastChecked<UBorshAttributeSet>(AttributeSet);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
	BorshAttributeSet->GetHealthAttribute()).AddUObject(this, &UOverlayWidgetController::HealthChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
	BorshAttributeSet->GetMaxHealthAttribute()).AddUObject(this, &UOverlayWidgetController::MaxHealthChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
	BorshAttributeSet->GetManaAttribute()).AddUObject(this, &UOverlayWidgetController::ManaChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
	BorshAttributeSet->GetMaxManaAttribute()).AddUObject(this, &UOverlayWidgetController::MaxManaChanged);

	// this function makes things simplier for us bcs we don't have to go and declare callback functions for all the delegates that we want to bind to.
	Cast<UBorshAbilitySystemComponent>(AbilitySystemComponent)->EffectAssetTags.AddLambda/*Anon function (doesn't have a name, not declared and not a member function */(
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

void UOverlayWidgetController::HealthChanged(const FOnAttributeChangeData& Data) const
{
	OnHealthChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::MaxHealthChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxHealthChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::ManaChanged(const FOnAttributeChangeData& Data) const
{
	OnManaChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::MaxManaChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxManaChanged.Broadcast(Data.NewValue);
}
 