#include "Character/BorshCharacter.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/BorshAbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Player/BorshPlayerState.h"
#include "NiagaraComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "UI/HUD/BorshHUD.h"
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

int32 ABorshCharacter::GetAttributesPointsReward_Implementation(int32 Level) const
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
}

void ABorshCharacter::AddToAttributePoints_Implementation(int32 InAttributePoints)
{
	// Getting our PlayerState
	const ABorshPlayerState* BorshPlayerState = GetPlayerState<ABorshPlayerState>();
	check(BorshPlayerState);
}

void ABorshCharacter::AddToSpellPoints_Implementation(int32 InSpellPoints)
{
	// Getting our PlayerState
	const ABorshPlayerState* BorshPlayerState = GetPlayerState<ABorshPlayerState>();
	check(BorshPlayerState);
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
void ABorshCharacter::InitAbilityActorInfo()
{
	ABorshPlayerState* BorshPlayerState = GetPlayerState<ABorshPlayerState>();
	check(BorshPlayerState);
	BorshPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(BorshPlayerState, this);
	Cast<UBorshAbilitySystemComponent>(BorshPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();
	AbilitySystemComponent = BorshPlayerState->GetAbilitySystemComponent();
	AttributeSet = BorshPlayerState->GetAttributeSet();

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

