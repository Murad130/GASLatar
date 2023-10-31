// Copyright Latar

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BorshEffectActor.generated.h"

class UGameplayEffect;

UCLASS()
class GASLATAR_API ABorshEffectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ABorshEffectActor();

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	TSubclassOf<UGameplayEffect> InstantGameplayEffectClass;

};
