// Copyright Latar


#include "Character/BorshCharacterBase.h"


ABorshCharacterBase::ABorshCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

UAbilitySystemComponent* ABorshCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ABorshCharacterBase::BeginPlay()
{
	Super::BeginPlay();

}

void ABorshCharacterBase::InitAbilityActorInfo()
{

}
