// Copyright Latar

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "BorshPlayerController.generated.h"

class UNiagaraSystem;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UBorshInputConfig;
class UBorshAbilitySystemComponent;
class USplineComponent;
class UDamageTextComponent;
class IEnemyInterface;

/**
 *
 */
UCLASS()
class GASLATAR_API ABorshPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ABorshPlayerController();
	virtual void PlayerTick(float DeltaTime) override;

	UFUNCTION(Client, Reliable)
	void ShowDamageNumber(float DamageAmount, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit);

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> BorshContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> ShiftAction;

	void ShiftPressed() { bShiftKeyDown = true; };
	void ShiftReleased() { bShiftKeyDown = false; };
	bool bShiftKeyDown = false;

	void Move(const FInputActionValue& InputActionValue);

	// Keeping track of Hovered enemies
	void CursorTrace();

	IEnemyInterface* LastActor;
	IEnemyInterface* ThisActor;
	FHitResult CursorHit;

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UBorshInputConfig> InputConfig;

	// Variable to store Ability System Component so we don't have to cast more than a single time (or else it might be expensive)
	UPROPERTY()
	TObjectPtr<UBorshAbilitySystemComponent> BorshAbilitySystemComponent;
	UBorshAbilitySystemComponent* GetASC();

	// Click To Move
	FVector CachedDestination = FVector::ZeroVector;
	// Keeping track of how long we've pressed the mouse button before we release so we can know if it was a short press
	float FollowTime = 0.f;
	float ShortPressThreshold = 0.5f;
	// Now we need to know when we're auto running. Ex: In other words, it was a short press and we like to generate some path points
	// and have our character moving along some kind of a spline that we're going to create that's going to be when we're auto running.
	bool bAutoRunning = false;
	//Boolean for whether or not we're targeting something.
	bool bTargeting = false;
	// Now we need to know at which point we should stop running (How close we get to that destination should be a parameter we can change).
	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius = 50.f;
	// Now we need a spline. A spline is going to allow us to generate a nice smooth cure like when we are going around of objects 
	// so our player doesn't make rough movements
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline; // And if we have a spline component variable, we need to construct that. We do that in the constructor.

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem> ClickNiagaraSystem;

	void AutoRun();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTextComponent> DamageTextComponentClass;

};