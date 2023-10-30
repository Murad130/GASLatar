
#include "Actor/BorshEffectActor.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/BorshAttributeSet.h"
#include "Components/SphereComponent.h"

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
	//TODO: Change this to apply a Gameplay Effect. For now, using const_cast as a hack!
	if (IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(OtherActor))
	{
		const UBorshAttributeSet* BorshAttributeSet = Cast<UBorshAttributeSet>(ASCInterface->GetAbilitySystemComponent()->GetAttributeSet(UBorshAttributeSet::StaticClass()));

		UBorshAttributeSet* MutableBorshAttributeSet = const_cast<UBorshAttributeSet*>(BorshAttributeSet);
		MutableBorshAttributeSet->SetHealth(BorshAttributeSet->GetHealth() + 25.f);
		MutableBorshAttributeSet->SetMana(BorshAttributeSet->GetMana() - 25.f);
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