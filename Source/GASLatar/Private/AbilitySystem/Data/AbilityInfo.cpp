// Copyright Latar


#include "AbilitySystem/Data/AbilityInfo.h"

#include "BorshLogChannels.h"

FBorshAbilityInfo UAbilityInfo::FindAbilityInfoForTag(const FGameplayTag& AbilityTag, bool bLogNotFound) const
{
	for (const FBorshAbilityInfo& Info : AbilityInformation)
	{
		if (Info.AbilityTag == AbilityTag)
		{
			return Info;
		}
	}

	if (bLogNotFound)
	{
		//UE_LOG(LogTemp, )
		UE_LOG(LogBorsh, Error, TEXT("Can't find Info for AttributeTag [%s] on AttributeInfo [%s]."), *AbilityTag.ToString(), *GetNameSafe(this));
	}
	return FBorshAbilityInfo();
}
