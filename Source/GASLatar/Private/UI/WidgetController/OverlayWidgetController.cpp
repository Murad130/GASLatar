// Copyright Latar


#include "UI/WidgetController/OverlayWidgetController.h"
#include <AbilitySystem/BorshAttributeSet.h>

void UOverlayWidgetController::BroadcastInitialValues()
{
	const UBorshAttributeSet* BorshAttributeSet = CastChecked<UBorshAttributeSet>(AttributeSet);

	OnHealthChanged.Broadcast(BorshAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(BorshAttributeSet->GetMaxHealth());
	
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	const UBorshAttributeSet* BorshAttributeSet = CastChecked<UBorshAttributeSet>(AttributeSet);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		BorshAttributeSet->GetHealthAttribute()).AddUObject(this, &UOverlayWidgetController::HealthChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		BorshAttributeSet->GetMaxHealthAttribute()).AddUObject(this, &UOverlayWidgetController::MaxHealthChanged);
}

void UOverlayWidgetController::HealthChanged(const FOnAttributeChangeData& Data) const
{
	OnHealthChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::MaxHealthChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxHealthChanged.Broadcast(Data.NewValue);
}
 