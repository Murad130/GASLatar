// Copyright Latar


#include "AbilitySystem/BorshAbilitySystemLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "BorshAbilityTypes.h"
#include "BorshGameplayTags.h"
#include "Game/BorshGameModeBase.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "UI/WidgetController/BorshWidgetController.h"
#include "UI/HUD/BorshHUD.h"
#include "Player/BorshPlayerState.h"

bool UBorshAbilitySystemLibrary::MakeWidgetControllerParams(const UObject* WorldContextObject, FWidgetControllerParams& OutWCParams, ABorshHUD*& OutBorshHUD)
{
	// Here we need to somehow, someway get the widget controller and return it to the caller. 
	// So how do we do that? Well, we need the player controller. So if we're calling this from within a widget, we're calling it from the perspective of a local player,
	// A widget will only exist for the local player and we'll want the player controller associated with that
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		OutBorshHUD = Cast<ABorshHUD>(PC->GetHUD());
		if (OutBorshHUD)
		{
			// now we have the HUD we want to call get overlay widget controller on the HUD.
			// But this getter requires 4 params (right now we have 1 param->PlayerController so first we need those variables
			ABorshPlayerState* PS = PC->GetPlayerState<ABorshPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();

			OutWCParams.AttributeSet = AS;
			OutWCParams.AbilitySystemComponent = ASC;
			OutWCParams.PlayerState = PS;
			OutWCParams.PlayerController = PC;
			return true;
		}
	}
	return false;
}

UOverlayWidgetController* UBorshAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	ABorshHUD* BorshHUD = nullptr;
	if (MakeWidgetControllerParams(WorldContextObject, WCParams, BorshHUD)) // same as || const bool bSuccessfulParams = MakeWidgetControllerParams(WorldContextObject, WCParams, BorshHUD) and use bSuccessfulParams in if();
	{
		return BorshHUD->GetOverlayWidgetController(WCParams);
	}
	return nullptr;
}

UAttributeMenuWidgetController* UBorshAbilitySystemLibrary::GetAttributeMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	ABorshHUD* BorshHUD = nullptr;
	if (MakeWidgetControllerParams(WorldContextObject, WCParams, BorshHUD)) // same as || const bool bSuccessfulParams = MakeWidgetControllerParams(WorldContextObject, WCParams, BorshHUD) and use bSuccessfulParams in if();
	{
		return BorshHUD->GetAttributeMenuWidgetController(WCParams);
	}
	return nullptr;
}

USpellMenuWidgetController* UBorshAbilitySystemLibrary::GetSpellMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	ABorshHUD* BorshHUD = nullptr;
	if (MakeWidgetControllerParams(WorldContextObject, WCParams, BorshHUD)) // same as || const bool bSuccessfulParams = MakeWidgetControllerParams(WorldContextObject, WCParams, BorshHUD) and use bSuccessfulParams in if();
	{
		return BorshHUD->GetSpellMenuWidgetController(WCParams);
	}
	return nullptr;
}

void UBorshAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC)
{
	AActor* AvatarActor = ASC->GetAvatarActor();

	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	FCharacterClassDefaultInfo ClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);

	FGameplayEffectContextHandle PrimaryAttributesContextHandle = ASC->MakeEffectContext();
	PrimaryAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle PrimaryAttributesSpecHandle = ASC->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttributes, Level, PrimaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle SecondaryAttributesContextHandle = ASC->MakeEffectContext();
	SecondaryAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle SecondaryAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttributes, Level, SecondaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle VitalAttributesContextHandle = ASC->MakeEffectContext();
	VitalAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle VitalAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->VitalAttributes, Level, VitalAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalAttributesSpecHandle.Data.Get());
}

void UBorshAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr) return;
	for (TSubclassOf<UGameplayAbility> AbilityClass : CharacterClassInfo->CommonAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		ASC->GiveAbility(AbilitySpec);
	}
	const FCharacterClassDefaultInfo& DefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
	for (TSubclassOf<UGameplayAbility> AbilityClass : DefaultInfo.StartupAbilities)
	{
		// Get player level
		if (ASC->GetAvatarActor()->Implements<UCombatInterface>())
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, ICombatInterface::Execute_GetPlayerLevel(ASC->GetAvatarActor()));
			ASC->GiveAbility(AbilitySpec);
		}
	}
}

int32 UBorshAbilitySystemLibrary::GetXPRewardForClassAndLevel(const UObject* WorldContextObject, ECharacterClass CharacterClass, int32 CharacterLevel)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr) return 0;

	const FCharacterClassDefaultInfo& Info = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
	const float XPReward = Info.XPReward.GetValueAtLevel(CharacterLevel);

	return static_cast<int32>(XPReward);
}

void UBorshAbilitySystemLibrary::SetIsRadialDamageEffectParam(UPARAM(ref)FDamageEffectParams& DamageEffectParams, bool bIsRadial, float InnerRadius, float OuterRadius, FVector Origin)
{
	DamageEffectParams.bIsRadialDamage = bIsRadial;
	DamageEffectParams.RadialDamageInnerRadius = InnerRadius;
	DamageEffectParams.RadialDamageOuterRadius = OuterRadius;
	DamageEffectParams.RadialDamageOrigin = Origin;
}

void UBorshAbilitySystemLibrary::SetKnockbackDirection(UPARAM(ref)FDamageEffectParams& DamageEffectParams, FVector KnockbackDirection, float Magnitude)
{
	KnockbackDirection.Normalize();
	if (Magnitude == 0.f)
	{
		DamageEffectParams.KnockbackForce = KnockbackDirection * DamageEffectParams.KnockbackForceMagnitude;
	}
	else
	{
		DamageEffectParams.KnockbackForce = KnockbackDirection * Magnitude;
	}
}

void UBorshAbilitySystemLibrary::SetDeathImpulseDirection(UPARAM(ref)FDamageEffectParams& DamageEffectParams, FVector ImpulseDirection, float Magnitude)
{
	ImpulseDirection.Normalize();
	if (Magnitude == 0.f)
	{
		DamageEffectParams.DeathImpulse = ImpulseDirection * DamageEffectParams.DeathImpulseMagnitude;
	}
	else
	{
		DamageEffectParams.DeathImpulse = ImpulseDirection * Magnitude;
	}
}

void UBorshAbilitySystemLibrary::SetTargetEffectParamsASC(UPARAM(ref)FDamageEffectParams& DamageEffectParams, UAbilitySystemComponent* InASC)
{
	DamageEffectParams.TargetAbilitySystemComponent = InASC;
}

UCharacterClassInfo* UBorshAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	const ABorshGameModeBase* BorshGameMode = Cast<ABorshGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (BorshGameMode == nullptr) return nullptr;
	return BorshGameMode->CharacterClassInfo;
}

UAbilityInfo* UBorshAbilitySystemLibrary::GetAbilityInfo(const UObject* WorldContextObject)
{
	const ABorshGameModeBase* BorshGameMode = Cast<ABorshGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (BorshGameMode == nullptr) return nullptr;
	return BorshGameMode->AbilityInfo;
}

bool UBorshAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FBorshGameplayEffectContext* BorshEffectContext = static_cast<const FBorshGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return BorshEffectContext->IsBlockedHit();
	}
	return false;
}

bool UBorshAbilitySystemLibrary::IsSuccessfulDebuff(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FBorshGameplayEffectContext* BorshEffectContext = static_cast<const FBorshGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return BorshEffectContext->IsSuccessfulDebuff();
	}
	return false;
}

float UBorshAbilitySystemLibrary::GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FBorshGameplayEffectContext* BorshEffectContext = static_cast<const FBorshGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return BorshEffectContext->GetDebuffDamage();
	}
	return 0.f;
}

float UBorshAbilitySystemLibrary::GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FBorshGameplayEffectContext* BorshEffectContext = static_cast<const FBorshGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return BorshEffectContext->GetDebuffDuration();
	}
	return 0.f;
}

float UBorshAbilitySystemLibrary::GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FBorshGameplayEffectContext* BorshEffectContext = static_cast<const FBorshGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return BorshEffectContext->GetDebuffFrequency();
	}
	return 0.f;
}

FGameplayTag UBorshAbilitySystemLibrary::GetDamageType(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FBorshGameplayEffectContext* BorshEffectContext = static_cast<const FBorshGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		if (BorshEffectContext->GetDamageType().IsValid())
		{
			return *BorshEffectContext->GetDamageType();
		}
	}
	return FGameplayTag();
}

FVector UBorshAbilitySystemLibrary::GetDeathImpulse(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FBorshGameplayEffectContext* BorshEffectContext = static_cast<const FBorshGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return BorshEffectContext->GetDeathImpulse();
	}
	return FVector::ZeroVector;
}

FVector UBorshAbilitySystemLibrary::GetKnockbackForce(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FBorshGameplayEffectContext* BorshEffectContext = static_cast<const FBorshGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return BorshEffectContext->GetKnockbackForce();
	}
	return FVector::ZeroVector;
}

bool UBorshAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FBorshGameplayEffectContext* BorshEffectContext = static_cast<const FBorshGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return BorshEffectContext->IsCriticalHit();
	}
	return false;
}

bool UBorshAbilitySystemLibrary::IsRadialDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FBorshGameplayEffectContext* BorshEffectContext = static_cast<const FBorshGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return BorshEffectContext->IsRadialDamage();
	}
	return false;
}

float UBorshAbilitySystemLibrary::GetRadialDamageInnerRadius(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FBorshGameplayEffectContext* BorshEffectContext = static_cast<const FBorshGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return BorshEffectContext->GetRadialDamageInnerRadius();
	}
	return 0.f;
}

float UBorshAbilitySystemLibrary::GetRadialDamageOuterRadius(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FBorshGameplayEffectContext* BorshEffectContext = static_cast<const FBorshGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return BorshEffectContext->GetRadialDamageOuterRadius();
	}
	return 0.f;
}

FVector UBorshAbilitySystemLibrary::GetRadialDamageOrigin(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FBorshGameplayEffectContext* BorshEffectContext = static_cast<const FBorshGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return BorshEffectContext->GetRadialDamageOrigin();
	}
	return FVector::ZeroVector;
}

void UBorshAbilitySystemLibrary::SetIsBlockedHit(UPARAM(ref)FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit)
{
	if (FBorshGameplayEffectContext* BorshEffectContext = static_cast<FBorshGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		BorshEffectContext->SetIsBlockedHit(bInIsBlockedHit);
	}
}

void UBorshAbilitySystemLibrary::SetIsCriticalHit(UPARAM(ref)FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit)
{
	if (FBorshGameplayEffectContext* BorshEffectContext = static_cast<FBorshGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		BorshEffectContext->SetIsCriticalHit(bInIsCriticalHit);
	}
}

void UBorshAbilitySystemLibrary::SetIsSuccessfulDebuff(UPARAM(ref)FGameplayEffectContextHandle& EffectContextHandle, bool bInSuccessfulDebuff)
{
	if (FBorshGameplayEffectContext* BorshEffectContext = static_cast<FBorshGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		BorshEffectContext->SetIsSuccessfulDebuff(bInSuccessfulDebuff);
	}
}

void UBorshAbilitySystemLibrary::SetDebuffDamage(UPARAM(ref)FGameplayEffectContextHandle& EffectContextHandle, float InDamage)
{
	if (FBorshGameplayEffectContext* BorshEffectContext = static_cast<FBorshGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		BorshEffectContext->SetDebuffDamage(InDamage);
	}
}

void UBorshAbilitySystemLibrary::SetDebuffDuration(UPARAM(ref)FGameplayEffectContextHandle& EffectContextHandle, float InDuration)
{
	if (FBorshGameplayEffectContext* BorshEffectContext = static_cast<FBorshGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		BorshEffectContext->SetDebuffDuration(InDuration);
	}
}

void UBorshAbilitySystemLibrary::SetDebuffFrequency(UPARAM(ref)FGameplayEffectContextHandle& EffectContextHandle, float InFrequency)
{
	if (FBorshGameplayEffectContext* BorshEffectContext = static_cast<FBorshGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		BorshEffectContext->SetDebuffFrequency(InFrequency);
	}
}

void UBorshAbilitySystemLibrary::SetDamageType(UPARAM(ref)FGameplayEffectContextHandle& EffectContextHandle, const FGameplayTag& InDamageType)
{
	if (FBorshGameplayEffectContext* BorshEffectContext = static_cast<FBorshGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		TSharedPtr<FGameplayTag> DamageType = MakeShared<FGameplayTag>(InDamageType);
		BorshEffectContext->SetDamageType(DamageType);
	}
}

void UBorshAbilitySystemLibrary::SetDeathImpulse(UPARAM(ref)FGameplayEffectContextHandle& EffectContextHandle, const FVector& InImpulse)
{
	if (FBorshGameplayEffectContext* BorshEffectContext = static_cast<FBorshGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		BorshEffectContext->SetDeathImpulse(InImpulse);
	}
}

void UBorshAbilitySystemLibrary::SetKnockbackForce(UPARAM(ref)FGameplayEffectContextHandle& EffectContextHandle, const FVector& InForce)
{
	if (FBorshGameplayEffectContext* BorshEffectContext = static_cast<FBorshGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		BorshEffectContext->SetKnockbackForce(InForce);
	}
}

void UBorshAbilitySystemLibrary::SetIsRadialDamage(UPARAM(ref)FGameplayEffectContextHandle& EffectContextHandle, bool bInIsRadialDamage)
{
	if (FBorshGameplayEffectContext* BorshEffectContext = static_cast<FBorshGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		BorshEffectContext->SetIsRadialDamage(bInIsRadialDamage);
	}
}

void UBorshAbilitySystemLibrary::SetRadialDamageInnerRadius(UPARAM(ref)FGameplayEffectContextHandle& EffectContextHandle, float InInnerRadius)
{
	if (FBorshGameplayEffectContext* BorshEffectContext = static_cast<FBorshGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		BorshEffectContext->SetRadialDamageInnerRadius(InInnerRadius);
	}
}

void UBorshAbilitySystemLibrary::SetRadialDamageOuterRadius(UPARAM(ref)FGameplayEffectContextHandle& EffectContextHandle, float InOuterRadius)
{
	if (FBorshGameplayEffectContext* BorshEffectContext = static_cast<FBorshGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		BorshEffectContext->SetRadialDamageOuterRadius(InOuterRadius);
	}
}

void UBorshAbilitySystemLibrary::SetRadialDamageOrigin(UPARAM(ref)FGameplayEffectContextHandle& EffectContextHandle, const FVector& InOrigin)
{
	if (FBorshGameplayEffectContext* BorshEffectContext = static_cast<FBorshGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		BorshEffectContext->SetRadialDamageOrigin(InOrigin);
	}
}

void UBorshAbilitySystemLibrary::GetLivePlayersWithinRadius(const UObject* WorldContextObject, TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius, const FVector& SphereOrigin)
{
	// We need to perform a check for a sphere in the world and get all actors inside of that sphere
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore);

	TArray<FOverlapResult> Overlaps;
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		World->OverlapMultiByObjectType(Overlaps, SphereOrigin, FQuat::Identity, FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects), FCollisionShape::MakeSphere(Radius), SphereParams);
		for (FOverlapResult& Overlap : Overlaps)
		{
			if (Overlap.GetActor()->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(Overlap.GetActor()))
			{
				OutOverlappingActors.AddUnique(ICombatInterface::Execute_GetAvatar(Overlap.GetActor()));
			}
		}
	}
}

void UBorshAbilitySystemLibrary::GetClosestTargets(int32 MaxTargets, const TArray<AActor*>& Actors, TArray<AActor*>& OutClosestTargets, const FVector& Origin)
{
	if (Actors.Num() <= MaxTargets)
	{
		OutClosestTargets = Actors;
		return;
	}

	TArray<AActor*> ActorsToCheck = Actors;
	int32 NumTargetsFound = 0;

	while (NumTargetsFound < MaxTargets)
	{
		if (ActorsToCheck.Num() == 0) break;
		double ClosestDistance = TNumericLimits<double>::Max();
		AActor* ClosestActor;
		for (AActor* PotentialTarget : ActorsToCheck)
		{
			const double Distance = (PotentialTarget->GetActorLocation() - Origin).Length();
			if (Distance < ClosestDistance)
			{
				ClosestDistance = Distance;
				ClosestActor = PotentialTarget;
			}
		}
		ActorsToCheck.Remove(ClosestActor);
		OutClosestTargets.AddUnique(ClosestActor);
		++NumTargetsFound;
	}
}

bool UBorshAbilitySystemLibrary::IsNotFriend(AActor* FirstActor, AActor* SecondActor)
{
	const bool bBothArePlayers = FirstActor->ActorHasTag(FName("Player")) && SecondActor->ActorHasTag(FName("Player"));
	const bool bBothAreEnemies = FirstActor->ActorHasTag(FName("Enemy")) && SecondActor->ActorHasTag(FName("Enemy"));
	const bool bFriends = bBothArePlayers || bBothAreEnemies;
	return !bFriends;
}

FGameplayEffectContextHandle UBorshAbilitySystemLibrary::ApplyDamageEffect(const FDamageEffectParams& DamageEffectParams)
{
	const FBorshGameplayTags& GameplayTags = FBorshGameplayTags::Get();
	const AActor* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();

	FGameplayEffectContextHandle EffectContexthandle = DamageEffectParams.SourceAbilitySystemComponent->MakeEffectContext();
	EffectContexthandle.AddSourceObject(SourceAvatarActor);
	SetDeathImpulse(EffectContexthandle, DamageEffectParams.DeathImpulse);
	SetKnockbackForce(EffectContexthandle, DamageEffectParams.KnockbackForce);

	SetIsRadialDamage(EffectContexthandle, DamageEffectParams.bIsRadialDamage);
	SetRadialDamageInnerRadius(EffectContexthandle, DamageEffectParams.RadialDamageInnerRadius);
	SetRadialDamageOuterRadius(EffectContexthandle, DamageEffectParams.RadialDamageOuterRadius);
	SetRadialDamageOrigin(EffectContexthandle, DamageEffectParams.RadialDamageOrigin);

	const FGameplayEffectSpecHandle SpecHandle = DamageEffectParams.SourceAbilitySystemComponent->MakeOutgoingSpec(DamageEffectParams.DamageGameplayEffectClass, DamageEffectParams.AbilityLevel, EffectContexthandle);

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageEffectParams.DamageType, DamageEffectParams.BaseDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Chance, DamageEffectParams.DebuffChance);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Damage, DamageEffectParams.DebuffDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Duration, DamageEffectParams.DebuffDuration);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Frequency, DamageEffectParams.DebuffFrequency);

	DamageEffectParams.TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	return EffectContexthandle;
}

TArray<FRotator> UBorshAbilitySystemLibrary::EvenlySpacedRotators(const FVector& Forward, const FVector& Axis, float Spread, int32 NumRotators)
{
	TArray<FRotator> Rotators;

	const FVector LeftOfSpread = Forward.RotateAngleAxis(-Spread / 2.f, Axis);
	if (NumRotators > 1)
	{
		const float DeltaSpread = Spread / (NumRotators - 1);
		for (int32 i = 0; i < NumRotators; i++)
		{
			const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
			Rotators.Add(Direction.Rotation());
		}
	}
	else
	{
		Rotators.Add(Forward.Rotation());
	}
	return Rotators;
}

TArray<FVector> UBorshAbilitySystemLibrary::EvenlyRotatedVectors(const FVector& Forward, const FVector& Axis, float Spread, int32 NumVectors)
{
	 TArray<FVector> Vectors;

	const FVector LeftOfSpread = Forward.RotateAngleAxis(-Spread / 2.f, Axis);
	if (NumVectors > 1)
	{
		const float DeltaSpread = Spread / (NumVectors - 1);
		for (int32 i = 0; i < NumVectors; i++)
		{
			const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
			Vectors.Add(Direction);
		}
	}
	else
	{
		Vectors.Add(Forward);
	}
	return Vectors;
}


