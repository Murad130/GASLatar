// Copyright Latar


#include "Player/BorshPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Input/BorshInputComponent.h"
#include "AbilitySystem/BorshAbilitySystemComponent.h"
#include <Interaction/EnemyInterface.h>
#include <EnhancedInputSubsystems.h>



ABorshPlayerController::ABorshPlayerController()
{
	bReplicates = true;
}

void ABorshPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
}

void ABorshPlayerController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit) return;

	LastActor = ThisActor;
	ThisActor = Cast<IEnemyInterface>(CursorHit.GetActor());

	/**
	* Line trace frim cursor, There are several scenarios:
	* 
	* A. LastActor is null && ThisActor is null
	*	- Do nothing
	* B. LastActor is null && ThisActor is valid
	*	- Call Highlight ThisActor
	* C. LastActor is valid && ThisActor is null
	*	- Call UnHighlight LastActor
	* D. Both actors are valid, but LastActor != ThisActor
	*	- Call UnHighlight LastActor and Call Highlight ThisActor
	* E. Both actors are valid, but are the same actor
	*	- Do nothing
	*/

	if (LastActor == nullptr)
	{
		if (ThisActor != nullptr)
		{
			// Case B
			ThisActor->HighlightActor();
		}
		else
		{
			// Case A - both are null, do nothing 

		}
	}
	else // LastActor is valid
	{
		if (ThisActor == nullptr)
		{
			// Case C
			LastActor->UnHighlightActor();
		}
		else // both actors are valid
		{
			if (LastActor != ThisActor)
			{
				// Case D
				LastActor->UnHighlightActor();
				ThisActor->HighlightActor();
			}
			else
			{
				// Case E - do nothing 

			}

		}
	}
}

void ABorshPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{

}

void ABorshPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (GetASC() == nullptr) return;
	GetASC()->AbilityInputTagReleased(InputTag);
}

void ABorshPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	// Now I don't want to have to cast every single time as ability input tag held may be called every frame and that can be expensive.
	if (GetASC() == nullptr) return;
	GetASC()->AbilityInputTagHeld(InputTag);
}

UBorshAbilitySystemComponent* ABorshPlayerController::GetASC()
{
	if (BorshAbilitySystemComponent == nullptr)
	{
		BorshAbilitySystemComponent = Cast<UBorshAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}
	return BorshAbilitySystemComponent;
}

void ABorshPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(BorshContext);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
	{
		Subsystem->AddMappingContext(BorshContext, 0);
	}

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void ABorshPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UBorshInputComponent* BorshInputComponent = CastChecked<UBorshInputComponent>(InputComponent);
	BorshInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABorshPlayerController::Move);
	// Now we need to call our function BindAbilityActions
	BorshInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}

void ABorshPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

