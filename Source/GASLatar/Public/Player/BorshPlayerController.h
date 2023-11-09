// Copyright Latar

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "BorshPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class IEnemyInterface;
class UBorshInputConfig;
class UBorshAbilitySystemComponent;

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


protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	UPROPERTY(EditAnywhere, Category = "Input")
		TObjectPtr<UInputMappingContext> BorshContext;

	UPROPERTY(EditAnywhere, Category = "Input")
		TObjectPtr<UInputAction> MoveAction;

	void Move(const FInputActionValue& InputActionValue);

	void CursorTrace();

	TObjectPtr<IEnemyInterface> LastActor;
	TObjectPtr<IEnemyInterface> ThisActor;

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UBorshInputConfig> InputConfig;

	// Variable to store Ability System Component so we don't have to cast more than a single time (or else it might be expensive)
	UPROPERTY()
	TObjectPtr<UBorshAbilitySystemComponent> BorshAbilitySystemComponent;
	UBorshAbilitySystemComponent* GetASC();

};
