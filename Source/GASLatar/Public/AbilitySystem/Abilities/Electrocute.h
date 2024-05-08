// Copyright Latar

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/BorshBeamSpell.h"
#include "Electrocute.generated.h"

/**
 * 
 */
UCLASS()
class GASLATAR_API UElectrocute : public UBorshBeamSpell
{
	GENERATED_BODY()
	
public:
	virtual FString GetDescription(int32 Level) override;
	virtual FString GetNextLevelDescription(int32 Level) override;
};
