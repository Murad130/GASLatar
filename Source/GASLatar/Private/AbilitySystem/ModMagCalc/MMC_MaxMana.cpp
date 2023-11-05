

// Copyright Latar


#include "AbilitySystem/ModMagCalc/MMC_MaxMana.h"

#include "AbilitySystem/BorshAttributeSet.h"
#include "Interaction/CombatInterface.h"

UMMC_MaxMana::UMMC_MaxMana()
{
	IntDef.AttributeToCapture = UBorshAttributeSet::GetIntelligenceAttribute();

	IntDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;

	IntDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(IntDef);
}

float UMMC_MaxMana::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// Gather tags from Source and Target
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	// how are we going to get that actual value for Int? Well, we pass in a float by reference and it's non-const reference. 
	// This function is going to set that float equal to the magnitude or value of Int at the time it was captured.
	float Int = 0.f;
	GetCapturedAttributeMagnitude(IntDef, Spec, EvaluationParameters, Int);
	// Now we have the value of Int. That's step one
	Int = FMath::Max<float>(Int, 0.f);

	// But as I mentioned, we want max health to be dependent not only on Int but also on the player's level. And that's the whole reason we created the combat interface, 
	// because we can always get the source object of this gameplay effect. We can get that from the spec
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(Spec.GetContext().GetSourceObject());
	const int32 PlayerLevel = CombatInterface->GetPlayerLevel();

	return 50.f + 2.5f * Int + 15.f * PlayerLevel;
}
