// Copyright Latar


#include "AbilitySystem/Abilities/BorshProjectileSpell.h"
#include "Actor/BorshProjectile.h"
#include "Interaction/CombatInterface.h"

void UBorshProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// We're going to spawn a projectile, but only if we're on the server.

	const bool bIsServer = HasAuthority(&ActivationInfo);
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

		// Now we want to spawn a projectile. But I'm interested in spawning an actor that's going to fly through the air and hit something and apply a gameplay effect. 
		FTransform SpawnTransform;
		// Setting SpawnTransform Location
		SpawnTransform.SetLocation(SocketLocation);
		// TODO: Set the projectile Rotation

		ABorshProjectile* Projectile = GetWorld()->SpawnActorDeferred<ABorshProjectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);  // So now this is going to spawn an actor at this transform.

		// TODO: Give the Projectile a Gameplay Effect Spec for causing damage

		// And to finish spawning the projectile
		Projectile->FinishSpawning(SpawnTransform);
	}
}
