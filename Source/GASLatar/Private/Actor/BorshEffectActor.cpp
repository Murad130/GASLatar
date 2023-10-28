// Copyright Latar


#include "Actor/BorshEffectActor.h"
#include "Components/SphereComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "Components/PrimitiveComponent.h"
#include "AbilitySystem/BorshAttributeSet.h"

ABorshEffectActor::ABorshEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	Sphere->SetupAttachment(GetRootComponent());

}

void ABorshEffectActor::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(OtherActor))
	{
		const UBorshAttributeSet* BorshAttributeSet = Cast<UBorshAttributeSet>(ASCInterface->GetAbilitySystemComponent()->GetAttributeSet(UBorshAttributeSet::StaticClass()));

		UBorshAttributeSet* MutableBorshAttributeSet = const_cast<UBorshAttributeSet*>(BorshAttributeSet);
		MutableBorshAttributeSet->SetHealth(BorshAttributeSet->GetHealth() + 25.f);
		Destroy();
	}
}

void ABorshEffectActor::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void ABorshEffectActor::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &ABorshEffectActor::OnOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &ABorshEffectActor::EndOverlap);

}

