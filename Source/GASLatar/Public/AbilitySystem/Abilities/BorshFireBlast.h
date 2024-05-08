// Copyright Latar

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/BorshDamageGameplayAbility.h"
#include "BorshFireBlast.generated.h"

class ABorshFireBall;

/**
 * 
 */
UCLASS()
class GASLATAR_API UBorshFireBlast : public UBorshDamageGameplayAbility
{
	GENERATED_BODY()

public:
	virtual FString GetDescription(int32 Level) override;
	virtual FString GetNextLevelDescription(int32 Level) override;

	UFUNCTION(BlueprintCallable)
	TArray<ABorshFireBall*> SpawnFireBalls();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "FireBlast")
	int32 NumFireBalls = 12;
	
private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ABorshFireBall> FireBallClass;
	
};
