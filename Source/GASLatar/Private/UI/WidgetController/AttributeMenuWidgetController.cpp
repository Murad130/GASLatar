// Copyright Latar


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AbilitySystem/BorshAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "BorshGameplayTags.h"

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{

}

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	// First we need to get our attribute set and find out the value of one of its attributes (ex: value of Strength)

	// Getting our attribute set
	UBorshAttributeSet* AS = CastChecked<UBorshAttributeSet>(AttributeSet);

	// First of all check make sure our attribute info is set and valid.
	check(AttributeInfo);

	FBorshAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(FBorshGameplayTags::Get().Attributes_Primary_Strength);

	// We need to set attribute's value 
	Info.AttributeValue = AS->GetStrength();

	// Now we need to broadcast. So we need to declare a delegate capable of sending our Borsh attribute info struct up to our widgets. We can make this in our header file
	AttributeInfoDelegate.Broadcast(Info);
	// Now our widgets need to bind to this attribute info delegate before we broadcast our initial values
	// So there are a couple things to do in blueprint. First, we need our attribute info set on our attribute menu widget Controller (Blueprint Class) and then we need to make sure that we bind to this delegate in order to receive this info on the blueprint side.
	
	

}
