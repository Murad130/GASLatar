// Copyright Latar


#include "UI/WidgetController/OverlayWidgetController.h"
#include <AbilitySystem/BorshAttributeSet.h>

void UOverlayWidgetController::BroadcastInitialValues()
{
	const UBorshAttributeSet* BorshAttributeSet = CastChecked<UBorshAttributeSet>(AttributeSet);

	OnHealthChanged.Broadcast(BorshAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(BorshAttributeSet->GetMaxHealth());

}
 