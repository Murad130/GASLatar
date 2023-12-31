// Copyright Latar

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "BorshGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class GASLATAR_API UBorshGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	FGameplayTag StartupInputTag;

	virtual FString GetDescription(int32 Level);
	virtual FString GetNextLevelDescription(int32 Level);
	static FString GetLockedDescription(int32 Level);
	

};
