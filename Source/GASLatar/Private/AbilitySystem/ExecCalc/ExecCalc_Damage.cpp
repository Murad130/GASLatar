// Copyright Latar


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "BorshGameplayTags.h"
#include "AbilitySystem/BorshAttributeSet.h"

struct BorshDamageStatics
{
	// This is a kind of shorthand version of doing what we did in the MMC_MaxHealth class for Vigor
	// Well, we want to declare some new capture definitions and there's a macro for that.
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);

	BorshDamageStatics()
	{
		// Anything we put here in BorshDamageStatics, if we call DamageStatics to access it, then we'll get the same one every time.
		DEFINE_ATTRIBUTE_CAPTUREDEF(UBorshAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UBorshAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UBorshAttributeSet, ArmorPenetration, Source, false);
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
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
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

	// Get Damage Set By Caller Magnitude
	float Damage = Spec.GetSetByCallerMagnitude(FBorshGameplayTags::Get().Damage);

	// Capture Block Chance on Target, and determine if there was a successfull Block
	float TargetBlockChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluationParameters, TargetBlockChance);
	TargetBlockChance = FMath::Max<float>(TargetBlockChance, 0.f);
	
	const bool bBlocked = FMath::RandRange(1, 100) < TargetBlockChance;
	// If Block, halve the damage.
	Damage = bBlocked ? Damage / 2.f : Damage; // same as if (bBlocked) Damage /= 2.f;

	// Capture Armor 
	float TargetArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParameters, TargetArmor);
	TargetArmor = FMath::Max<float>(TargetArmor, 0.f);

	// Capture ArmorPenetration.
	float SourceArmorPenetration = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvaluationParameters, SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max<float>(SourceArmorPenetration, 0.f);

	// ArmorPenetration ignores a percentage of the target Armor.
	const float EffectiveArmor = TargetArmor *= (100 - SourceArmorPenetration * 0.25f) / 100.f;
	// Armor ignore a percentage of incoming damage
	Damage *= (100 - EffectiveArmor * 0.333f) / 100.f;

	// So one of the last things to know how to do is how do we determine what exactly happens as a result of this execution calculation ? execution output is related to that.
	// We modify attributes using OutExecutionOutput parameter of this function.
	const FGameplayModifierEvaluatedData EvaluatedData(UBorshAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage); 
	OutExecutionOutput.AddOutputModifier(EvaluatedData);

}
