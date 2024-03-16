#include "Character/BorshCharacter.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/BorshAbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Player/BorshPlayerState.h"
#include "NiagaraComponent.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "UI/HUD/BorshHUD.h"
#include "BorshGameplayTags.h"
#include "Player/BorshPlayerController.h"

ABorshCharacter::ABorshCharacter()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->bDoCollisionTest = false;

	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>("TopDownCameraComponent");
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false;

	LevelUpNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("LevelUpNiagaraComponent");
	LevelUpNiagaraComponent->SetupAttachment(GetRootComponent());
	LevelUpNiagaraComponent->bAutoActivate = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	CharacterClass = ECharacterClass::Elementalist;
}

void ABorshCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	// Init ability actor info for the Server
	InitAbilityActorInfo();
	// Adding Character Abilities
	AddCharacterAbilities();
}

void ABorshCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	// Init ability actor info for the Client
	InitAbilityActorInfo();
}

void ABorshCharacter::AddToXP_Implementation(int32 InXP)
{
	ABorshPlayerState* BorshPlayerState = GetPlayerState<ABorshPlayerState>();
	check(BorshPlayerState);
	BorshPlayerState->AddToXP(InXP);
}

void ABorshCharacter::LevelUp_Implementation()
{
	MulticastLevelUpParticles_Implementation();
}

void ABorshCharacter::MulticastLevelUpParticles_Implementation() const
{
	if (IsValid(LevelUpNiagaraComponent))
	{
		const FVector CameraLocation = TopDownCameraComponent->GetComponentLocation();
		const FVector NiagaraSystemLocation = LevelUpNiagaraComponent->GetComponentLocation();
		const FRotator ToCameraRotation = (CameraLocation - NiagaraSystemLocation).Rotation();
		LevelUpNiagaraComponent->SetWorldRotation(ToCameraRotation);
		LevelUpNiagaraComponent->Activate(true);
	}
}

int32 ABorshCharacter::FindLevelForXP_Implementation(int32 InXP) const
{
	// Getting our PlayerState
	const ABorshPlayerState* BorshPlayerState = GetPlayerState<ABorshPlayerState>();
	check(BorshPlayerState);
	return BorshPlayerState->LevelUpInfo->FindLevelForXP(InXP);
}

int32 ABorshCharacter::GetAttributePointsReward_Implementation(int32 Level) const
{
	// Getting our PlayerState
	const ABorshPlayerState* BorshPlayerState = GetPlayerState<ABorshPlayerState>();
	check(BorshPlayerState);
	return BorshPlayerState->LevelUpInfo->LevelUpInformation[Level].AttributePointAward;
}

int32 ABorshCharacter::GetSpellPointsReward_Implementation(int32 Level) const
{
	// Getting our PlayerState
	const ABorshPlayerState* BorshPlayerState = GetPlayerState<ABorshPlayerState>();
	check(BorshPlayerState);
	return BorshPlayerState->LevelUpInfo->LevelUpInformation[Level].SpellPointAward;
}

void ABorshCharacter::AddToPlayerLevel_Implementation(int32 InPlayerLevel)
{
	// Getting our PlayerState
	ABorshPlayerState* BorshPlayerState = GetPlayerState<ABorshPlayerState>();
	check(BorshPlayerState);
	BorshPlayerState->AddToLevel(InPlayerLevel);

	if (UBorshAbilitySystemComponent* BorshASC = Cast<UBorshAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		BorshASC->UpdateAbilityStatuses(BorshPlayerState->GetPlayerLevel());
	}
}

void ABorshCharacter::AddToAttributePoints_Implementation(int32 InAttributePoints)
{
	// Getting our PlayerState
	ABorshPlayerState* BorshPlayerState = GetPlayerState<ABorshPlayerState>();
	check(BorshPlayerState);
	BorshPlayerState->AddToAttributePoints(InAttributePoints);
}

void ABorshCharacter::AddToSpellPoints_Implementation(int32 InSpellPoints)
{
	// Getting our PlayerState
	ABorshPlayerState* BorshPlayerState = GetPlayerState<ABorshPlayerState>();
	check(BorshPlayerState);
	BorshPlayerState->AddToSpellPoints(InSpellPoints);
}

int32 ABorshCharacter::GetAttributePoints_Implementation() const
{
	ABorshPlayerState* BorshPlayerState = GetPlayerState<ABorshPlayerState>();
	check(BorshPlayerState);
	return BorshPlayerState->GetAttributePoints();
}

int32 ABorshCharacter::GetSpellPoints_Implementation() const
{
	ABorshPlayerState* BorshPlayerState = GetPlayerState<ABorshPlayerState>();
	check(BorshPlayerState);
	return BorshPlayerState->GetSpellPoints();
}

int32 ABorshCharacter::GetXP_Implementation() const
{
	// Getting our PlayerState
	const ABorshPlayerState* BorshPlayerState = GetPlayerState<ABorshPlayerState>();
	check(BorshPlayerState);
	return BorshPlayerState->GetXP();
}

int32 ABorshCharacter::GetPlayerLevel_Implementation()
{
	// Getting our PlayerState
	const ABorshPlayerState* BorshPlayerState = GetPlayerState<ABorshPlayerState>();
	check(BorshPlayerState);
	return BorshPlayerState->GetPlayerLevel();

}

void ABorshCharacter::OnRep_Stunned()
{
	if (UBorshAbilitySystemComponent* BorshASC = Cast<UBorshAbilitySystemComponent>(AbilitySystemComponent))
	{
		const FBorshGameplayTags& GameplayTags = FBorshGameplayTags::Get();
		FGameplayTagContainer BlockedTags;
		BlockedTags.AddTag(GameplayTags.Player_Block_CursorTrace);
		BlockedTags.AddTag(GameplayTags.Player_Block_InputHeld);
		BlockedTags.AddTag(GameplayTags.Player_Block_InputPressed);
		BlockedTags.AddTag(GameplayTags.Player_Block_InputReleased);
		if (bIsStunned)
		{
			BorshASC->AddLooseGameplayTags(BlockedTags);
			StunDebuffComponent->Activate();
		}
		else
		{
			BorshASC->RemoveLooseGameplayTags(BlockedTags);
			StunDebuffComponent->Deactivate();
		}
	}
}

void ABorshCharacter::OnRep_Burned()
{
	if (bIsBurned)
	{
		BurnDebuffComponent->Activate();
	}
	else
	{
		BurnDebuffComponent->Deactivate();
	}
}

void ABorshCharacter::InitAbilityActorInfo()
{
	ABorshPlayerState* BorshPlayerState = GetPlayerState<ABorshPlayerState>();
	check(BorshPlayerState);
	BorshPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(BorshPlayerState, this);
	Cast<UBorshAbilitySystemComponent>(BorshPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();
	AbilitySystemComponent = BorshPlayerState->GetAbilitySystemComponent();
	AttributeSet = BorshPlayerState->GetAttributeSet();
	OnAscRegistered.Broadcast(AbilitySystemComponent);
	AbilitySystemComponent->RegisterGameplayTagEvent(FBorshGameplayTags::Get().Debuff_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ABorshCharacter::StunTagChanged);

	if (ABorshPlayerController* BorshPlayerController = Cast<ABorshPlayerController>(GetController()))
	{
		if (ABorshHUD* BorshHUD = Cast<ABorshHUD>(BorshPlayerController->GetHUD()))
		{
			BorshHUD->InitOverlay(BorshPlayerController, BorshPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}
	// We called our function. So now all that's left is to make sure that we have that effect and that we configure some default values (in Blueprint). 
	InitializeDefaultAttributes();
}

