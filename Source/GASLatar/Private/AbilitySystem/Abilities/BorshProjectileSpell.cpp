// Copyright Latar

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Abilities/BorshProjectileSpell.h"
#include "Actor/BorshProjectile.h"
#include "Interaction/CombatInterface.h"
#include "GASLatar/Public/BorshGameplayTags.h"

void UBorshProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

}

void UBorshProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation)
{
	
	// We're going to spawn a projectile, but only if we're on the server.

	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer) return;

	// Accessing CombatInterface
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
	if (CombatInterface)
	{

		// We'de like to spawn this at the tip of the our weapon (Magic staff)
		// We need to be able to get a socket location from the weapon of whoever owns this.
		// We can create Interface that returns the socket location in our CombatInterface Class

		// Getting Socket Location
		const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();
		// Projectile Rotation
		FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
		// We want our projectiles to fly parallel to the ground
		Rotation.Pitch = 0.f;

		// Now we want to spawn a projectile. But I'm interested in spawning an actor that's going to fly through the air and hit something and apply a gameplay effect. 
		FTransform SpawnTransform;
		// Setting SpawnTransform Location
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(Rotation.Quaternion());

		ABorshProjectile* Projectile = GetWorld()->SpawnActorDeferred<ABorshProjectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);  // So now this is going to spawn an actor at this transform.

		// TODO: Give the Projectile a Gameplay Effect Spec for causing damage
		// We're going to first of all make an effect spec handle and that means we need the ability system component
		const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
		const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), SourceASC->MakeEffectContext());

		FBorshGameplayTags GameplayTags = FBorshGameplayTags::Get();
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Damage, 50.f);
		Projectile->DamageEffectSpecHandle = SpecHandle;

		// And to finish spawning the projectile
		Projectile->FinishSpawning(SpawnTransform);
	}

}
