// Copyright Latar


#include "Character/EnemyCharacter.h"
#include "AbilitySystem/BorshAbilitySystemLibrary.h"
#include "GASLatar/GASLatar.h"
#include "Components/WidgetComponent.h"
#include <AbilitySystem/BorshAbilitySystemComponent.h>
#include <AbilitySystem/BorshAttributeSet.h>
#include "UI/Widget/BorshUserWidget.h"
#include "BorshGameplayTags.h"
#include "GameFramework/CharacterMovementComponent.h"

AEnemyCharacter::AEnemyCharacter()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	AbilitySystemComponent = CreateDefaultSubobject<UBorshAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UBorshAttributeSet>("AttributeSet");

	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBar->SetupAttachment(GetRootComponent());

}
void AEnemyCharacter::HighlightActor()
{
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	Weapon->SetRenderCustomDepth(true);
	Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
}
void AEnemyCharacter::UnHighlightActor()
{
	GetMesh()->SetRenderCustomDepth(false);
	Weapon->SetRenderCustomDepth(false);
}

int32 AEnemyCharacter::GetPlayerLevel()
{
	return Level;
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	InitAbilityActorInfo();
	UBorshAbilitySystemLibrary::GiveStartupAbilities(this, AbilitySystemComponent);

	if (UBorshUserWidget* BorshUserWidget = Cast<UBorshUserWidget>(HealthBar->GetUserWidgetObject()))
	{
		BorshUserWidget->SetWidgetController(this);
	}

	if (const UBorshAttributeSet* BorshAS = Cast<UBorshAttributeSet>(AttributeSet))
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(BorshAS->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			}
		);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(BorshAS->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			}
		);

		// We need to add listen for a gameplay tag change on the ability system component. If a gameplay tag is added or removed, we can do something in response to that. We need to do that for the Hit React Tag

		// Binding a callback or a lambda to a delegate that's broadcast from the ability system component whenever a tag is added.
		AbilitySystemComponent->RegisterGameplayTagEvent(FBorshGameplayTags::Get().Effects_HitReact, EGameplayTagEventType::NewOrRemoved).AddUObject(
			this,
			&AEnemyCharacter::HitReactTagChanged
		);

		OnHealthChanged.Broadcast(BorshAS->GetHealth());
		OnMaxHealthChanged.Broadcast(BorshAS->GetMaxHealth());
	}

}

void AEnemyCharacter::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	 bHitReacting = NewCount > 0;
	 GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;
}


void AEnemyCharacter::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UBorshAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();

	InitializeDefaultAttributes();
}

void AEnemyCharacter::InitializeDefaultAttributes() const
{
	UBorshAbilitySystemLibrary::InitializeDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);
}

