// Copyright Latar


#include "Character/BorshCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/BorshAbilitySystemComponent.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "GASLatar/GASLatar.h"
#include "BorshGameplayTags.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

ABorshCharacterBase::ABorshCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	const FBorshGameplayTags& GameplayTags = FBorshGameplayTags::Get();

	BurnDebuffComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>("BurnDebuffComponent");
	BurnDebuffComponent->SetupAttachment(GetRootComponent());
	BurnDebuffComponent->DebuffTag = GameplayTags.Debuff_Burn;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

UAbilitySystemComponent* ABorshCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAnimMontage* ABorshCharacterBase::GetHitReactMontage_Implementation()
{
	return HitReactMontage;
}

void ABorshCharacterBase::Die(const FVector& DeathImpluse)
{
	// Drop the weapon (Detachment automatically replicated)
	Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	MulticastHandleDeath(DeathImpluse);
}

void ABorshCharacterBase::MulticastHandleDeath_Implementation(const FVector& DeathImpulse)
{
	UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation(), GetActorRotation());

	// Ragdoll Weapon
	Weapon->SetSimulatePhysics(true);
	Weapon->SetEnableGravity(true);
	Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	Weapon->AddImpulse(DeathImpulse * 0.1f, NAME_None, true);

	// Ragdoll Mesh
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetMesh()->AddImpulse(DeathImpulse, NAME_None, true);


	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Dissolve();
	bDead = true;
	BurnDebuffComponent->Deactivate();

}

void ABorshCharacterBase::BeginPlay()
{
	Super::BeginPlay();

}

FVector ABorshCharacterBase::GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag)
{
	// Return correct socket based on MontageTag
	const FBorshGameplayTags& GameplayTags = FBorshGameplayTags::Get();
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_Weapon) && IsValid(Weapon))
	{
		return Weapon->GetSocketLocation(WeaponTipSocketName);
	}
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_LeftHand))
	{
		return GetMesh()->GetSocketLocation(LeftHandSocketName);
	}
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_RightHand))
	{
		return GetMesh()->GetSocketLocation(RightHandSocketName);
	}
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_Tail))
	{
		return GetMesh()->GetSocketLocation(TailSocketName);
	}
	return FVector();
	
}

bool ABorshCharacterBase::IsDead_Implementation() const
{
	return bDead;
}

AActor* ABorshCharacterBase::GetAvatar_Implementation()
{
	return this;
}

TArray<FTaggedMontage> ABorshCharacterBase::GetAttackMontages_Implementation()
{
	return AttackMontages;
}

UNiagaraSystem* ABorshCharacterBase::GetBloodEffect_Implementation()
{
	return BloodEffect;
}

FTaggedMontage ABorshCharacterBase::GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag)
{
	for (FTaggedMontage TaggedMontage : AttackMontages)
	{
		if (TaggedMontage.MontageTag == MontageTag)
		{
			return TaggedMontage;
		}
	}
	return FTaggedMontage();
}

int32 ABorshCharacterBase::GetMinionCount_Implementation()
{
	return MinionCount;
}

void ABorshCharacterBase::IncrementMinionCount_Implementation(int32 Amount)
{
	MinionCount += Amount;
}

void ABorshCharacterBase::DecrementMinionCount_Implementation(int32 Amount)
{
	MinionCount -= Amount;
}

ECharacterClass ABorshCharacterBase::GetCharacterClass_Implementation()
{
	return CharacterClass;
}

FOnASCRegistered ABorshCharacterBase::GetOnASCRegisteredDelegate()
{
	return OnAscRegistered;
}

USkeletalMeshComponent* ABorshCharacterBase::GetWeapon_Implementation()
{
	return Weapon;
}

void ABorshCharacterBase::InitAbilityActorInfo()
{

}

void ABorshCharacterBase::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const
{
	// We shouldn't have any issues calling this function as long as our Ability System component is Valid and our GameplayEffectClass is not a null pointer. We can check those things.
	check(IsValid(GetAbilitySystemComponent()));
	check(GameplayEffectClass);
	// Making a context handle
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	ContextHandle.AddSourceObject(this);

	// Making effect spec (Effect spec requires gameplay effect (here: GameplayEffectClass), level and a context handle)
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass, Level, ContextHandle);

	// this function requires an effect spec and ability system component that is a TARGET and an (optional) key
	// We can't just pass in spec handle (we have buried deep down inside the spec handle) we need to call a variable that store the actual spec. The variable is Data
	// Now we have another problem. Data is also a wrapper of its own (it's a shared pointer) to get the actual pointer we have to use Get()
	// Now we have another problem. This function takes a const reference and not a pointer so we have to dereference the pointer by adding * (*SpecHandle.Data.Get())
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());

	// So now that we have this function, the question is where do we call it? Now we can call it in different circumstances depending on if we're the Borsh character or the enemy Character.
	// Unlike init ability actor info which we're calling on both server and client, applying this gameplay effect needs to be done only on the server. Why ? 
	// Because all of those attributes are marked to be replicated. So if we change them on the server then they'll change on the client as well.
}

void ABorshCharacterBase::InitializeDefaultAttributes() const
{
	ApplyEffectToSelf(DefaultPrimaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultSecondaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultVitalAttributes, 1.f);
}

void ABorshCharacterBase::AddCharacterAbilities()
{
	UBorshAbilitySystemComponent* BorshASC = CastChecked<UBorshAbilitySystemComponent>(AbilitySystemComponent);

	// Only adds abilities on the server(not replicated)
	// First we need to check for authority
	if (!HasAuthority()) return;
	// If we have authority, we'll call the function. But this means we need to access the ability system component and we're going to cast it.
	
	BorshASC->AddCharacterAbilities(StartupAbilities);
	BorshASC->AddCharacterPassiveAbilities(StartupPassiveAbilities);

	// So the question now is when do we call add character abilities (This function)? Well, a good place to call it is in possessed by.
}

void ABorshCharacterBase::Dissolve()
{
	if (IsValid(DissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DynamicMatInst);

		StartDissolveTimeLine(DynamicMatInst);
	}
	if (IsValid(WeaponDissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(WeaponDissolveMaterialInstance, this);
		Weapon->SetMaterial(0, DynamicMatInst);

		StartWeaponDissolveTimeLine(DynamicMatInst);
	}
}

