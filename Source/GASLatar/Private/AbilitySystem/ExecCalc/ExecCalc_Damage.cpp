// Copyright Latar


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/BorshAttributeSet.h"

struct BorshDamageStatics
{
	// This is a kind of shorthand version of doing what we did in the MMC_MaxHealth class for Vigor
	// Well, we want to declare some new capture definitions and there's a macro for that.
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);

	BorshDamageStatics()
	{
		// Anything we put here in BorshDamageStatics, if we call DamageStatics to access it, then we'll get the same one every time.
		DEFINE_ATTRIBUTE_CAPTUREDEF(UBorshAttributeSet, Armor, Target, false);
		// But we also have to add this to a list, specifically an array on the calc damage class to tell this execution calculation class that this capture def should be used to capture that specific attribute.
		// We need to do this in our Constructor (in UExecCalc_Damage::UExecCalc_Damage())
	}
};

static const BorshDamageStatics& DamageStatics()
{
	// So every time we call this function over and over again, we're going to return this DStatics and every time we call it, we'll get the same DStatics object.
	static BorshDamageStatics DStatics;
	return DStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	const AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	const AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	// Now the question how do we actually capture it here in our execution calculation function execute? How are we going to actually get that armor definition? Well, the way to do it is through a function on execution params
	float Armor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParameters, Armor);
	Armor = FMath::Max<float>(0.f, Armor); // So this would make sure that armor is never a negative value
	++Armor;

	// So one of the last things to know how to do is how do we determine what exactly happens as a result of this execution calculation ? execution output is related to that.
	// We modify attributes using OutExecutionOutput parameter of this function.
	const FGameplayModifierEvaluatedData EvaluatedData(DamageStatics().ArmorProperty, EGameplayModOp::Additive, Armor); // Now all we're doing here is taking the value of armor and passing it into this evaluated data. We're not actually changing its value
	OutExecutionOutput.AddOutputModifier(EvaluatedData);

}
