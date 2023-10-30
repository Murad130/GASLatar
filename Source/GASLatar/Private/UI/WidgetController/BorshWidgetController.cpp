// Copyright Latar


#include "UI/WidgetController/BorshWidgetController.h"

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
