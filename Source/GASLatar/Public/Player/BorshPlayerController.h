// Copyright Latar

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BorshPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
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
};
