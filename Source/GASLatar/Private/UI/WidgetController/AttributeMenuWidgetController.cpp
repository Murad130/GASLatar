// Copyright Latar


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AbilitySystem/BorshAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"


void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	// We also need to update the values as they change. So if strength changes, we want to broadcast the latest value of strength up to our widgets so they can update.
	// We can bind those callbacks/Lambdas here 
	UBorshAttributeSet* AS = CastChecked<UBorshAttributeSet>(AttributeSet);
	check(AttributeInfo);
	for (auto Pair : AS->TagsToAttributes)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
			[this, Pair](const FOnAttributeChangeData& Data)
			{
				BroadcastAttributeInfo(Pair.Key, Pair.Value());
			}	
		);
	}
}

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	/*I'd like to just have some kind of generic algorithm right here that doesn't care about what the attributes are or how many. So we don't have to do this (code below) for each attribute (13 attributes)


	// First we need to get our attribute set and find out the value of one of its attributes (ex: value of Strength)

	FBorshAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(FBorshGameplayTags::Get().Attributes_Primary_Strength);

	// We need to set attribute's value 
	Info.AttributeValue = AS->GetStrength();

	// Now we need to broadcast. So we need to declare a delegate capable of sending our Borsh attribute info struct up to our widgets. We can make this in our header file
	AttributeInfoDelegate.Broadcast(Info);
	// Now our widgets need to bind to this attribute info delegate before we broadcast our initial values
	// So there are a couple things to do in blueprint. First, we need our attribute info set on our attribute menu widget Controller (Blueprint Class) and then we need to make sure that we bind to this delegate in order to receive this info on the blueprint side.
	
	*/

	// Getting our attribute set
	UBorshAttributeSet* AS = CastChecked<UBorshAttributeSet>(AttributeSet);
	// First of all check make sure our attribute info is set and valid.
	check(AttributeInfo);
	// We need a map that maps tags to those attributes
	for (auto& Pair : AS->TagsToAttributes)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	}
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const
{
	// So for the attribute we pass in our pair key (that's the gameplay attribute) and find attribute info for tag will return that for attribute info.
	FBorshAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag);
	// But we also need the actual attribute value.
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	// But now we have our attribute info and we've set its attribute value and we can broadcast it
	AttributeInfoDelegate.Broadcast(Info);
}
