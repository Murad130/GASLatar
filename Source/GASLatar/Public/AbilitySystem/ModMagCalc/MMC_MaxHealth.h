// Copyright Latar

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_MaxHealth.generated.h"

/**
 * 
 */
UCLASS()
class GASLATAR_API UMMC_MaxHealth : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	// Constructor
	UMMC_MaxHealth();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
	// Capture attributes (struct that defines what attribute we're going to capture)
	// Right now it's just an empty variable. We can capture the vigor attribute in the constructor 
	FGameplayEffectAttributeCaptureDefinition VigorDef;


};
