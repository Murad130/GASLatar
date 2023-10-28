#include "Character/BorshCharacter.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/BorshPlayerState.h"

ABorshCharacter::ABorshCharacter()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
}

void ABorshCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Init ability actor info for the Server
	InitAbilityActorInfo();
}

void ABorshCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Init ability actor info for the Client
	InitAbilityActorInfo();
}

void ABorshCharacter::InitAbilityActorInfo()
{
	ABorshPlayerState* BorshPlayerState = GetPlayerState<ABorshPlayerState>();
	check(BorshPlayerState);
	BorshPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(BorshPlayerState, this);
	AbilitySystemComponent = BorshPlayerState->GetAbilitySystemComponent();
	AttributeSet = BorshPlayerState->GetAttributeSet();
}