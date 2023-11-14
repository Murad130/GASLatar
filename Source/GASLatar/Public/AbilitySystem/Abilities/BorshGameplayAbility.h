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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	FScalableFloat Damage;
	
};
