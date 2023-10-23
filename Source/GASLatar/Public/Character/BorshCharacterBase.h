// Copyright Latar

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BorshCharacterBase.generated.h"

UCLASS(Abstract)
class GASLATAR_API ABorshCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ABorshCharacterBase();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Combat")
		TObjectPtr<USkeletalMeshComponent> Weapon;

};
