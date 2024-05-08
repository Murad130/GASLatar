// Copyright Latar


#include "Actor/BorshFireBall.h"
#include "AbilitySystem/BorshAbilitySystemLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"

void ABorshFireBall::BeginPlay()
{
	Super::BeginPlay();
	StartOutgoingTimeline();
}

void ABorshFireBall::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValidOverlap(OtherActor)) return;

	if (HasAuthority())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			const FVector DeathImpulse = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;
			DamageEffectParams.DeathImpulse = DeathImpulse;

			DamageEffectParams.TargetAbilitySystemComponent = TargetASC;
			UBorshAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
		}
	}
}
