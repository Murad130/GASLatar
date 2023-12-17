// Copyright Latar


#include "AbilitySystem/ModMagCalc/MMC_MaxHealth.h"

#include "AbilitySystem/BorshAttributeSet.h"
#include "Interaction/CombatInterface.h"

UMMC_MaxHealth::UMMC_MaxHealth()
{
	VigorDef.AttributeToCapture = UBorshAttributeSet::GetVigorAttribute();
	// We have to define whether we're capturing vigor from the Target or the Source
	VigorDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	// Now we need to decide whether or not to Snapshot. Snapshot = When do we want to capture the attribute
	// When the gameplay effect spec is created or when the gameplay effect spec is applied?
	// Well, in our case, those two things happen one after the other. We create the gameplay effects back and we apply it immediately within the same function call, so it doesn't really matter.
	VigorDef.bSnapshot = false;

	// Adding our Attribute captured definition to MMC array 
	RelevantAttributesToCapture.Add(VigorDef);
}

float UMMC_MaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// Gather tags from Source and Target
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	// how are we going to get that actual value for vigor? Well, we pass in a float by reference and it's non-const reference. 
	// This function is going to set that float equal to the magnitude or value of vigor at the time it was captured.
	float Vigor = 0.f;
	GetCapturedAttributeMagnitude(VigorDef, Spec, EvaluationParameters, Vigor);
	// Now we have the value of vigor. That's step one
	Vigor = FMath::Max<float>(Vigor, 0.f);

	// But as I mentioned, we want max health to be dependent not only on vigor but also on the player's level. And that's the whole reason we created the combat interface, 
	// because we can always get the source object of this gameplay effect. We can get that from the spec
	// Old Version ICombatInterface* CombatInterface = Cast<ICombatInterface>(Spec.GetContext().GetSourceObject());
	// const int32 PlayerLevel = CombatInterface->GetPlayerLevel();
	int32 PlayerLevel = 1;
	if (Spec.GetContext().GetSourceObject()->Implements<UCombatInterface>())
	{
		PlayerLevel = ICombatInterface::Execute_GetPlayerLevel(Spec.GetContext().GetSourceObject());
	}

	return 80.f + 2.5f * Vigor + 10.f * PlayerLevel;
}
