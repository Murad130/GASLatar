// Copyright Latar

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/BorshProjectileSpell.h"
#include "BorshFireBolt.generated.h"

/**
 * 
 */
UCLASS()
class GASLATAR_API UBorshFireBolt : public UBorshProjectileSpell
{
	GENERATED_BODY()
	
public:
	virtual FString GetDescription(int32 Level) override;
	virtual FString GetNextLevelDescription(int32 Level) override;
};
