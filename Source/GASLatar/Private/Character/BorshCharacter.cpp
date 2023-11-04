#include "Character/BorshCharacter.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/BorshAbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/BorshPlayerState.h"
#include "UI/HUD/BorshHUD.h"
#include <Player/BorshPlayerController.h>

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
int32 ABorshCharacter::GetPlayerLevel()
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