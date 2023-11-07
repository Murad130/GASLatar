// Copyright Latar


#include "AbilitySystem/Data/AttributeInfo.h"

FBorshAttributeInfo UAttributeInfo::FindAttributeInfoForTag(const FGameplayTag& AttributeTag, bool bLogNotFound) const
{
	// So how are we going to find the ability info struct in our array that has this attribute tag?
	// Well, we can use a for loop.

	for (const FBorshAttributeInfo& Info : AttributeInformation)
	{
		if (Info.AttributeTag.MatchesTagExact(AttributeTag))
		{
			return Info;
		}
	}
	// if we never found an attribute info in the array we can log an error
	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find Info for AttributeTag [%s] on AttributeInfo [%s]."), *AttributeTag.ToString(), *GetNameSafe(this));
	}
	// We still need to return something. So in that case, we'll return an empty or attribute info.
	return FBorshAttributeInfo();
}
