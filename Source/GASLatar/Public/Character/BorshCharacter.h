// Copyright Latar

#pragma once

#include "CoreMinimal.h"
#include "Character/BorshCharacterBase.h"
#include "BorshCharacter.generated.h"

/**
 * 
 */
UCLASS()
class GASLATAR_API ABorshCharacter : public ABorshCharacterBase
{
	GENERATED_BODY()

public:
	ABorshCharacter();

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

private:
	void InitAbilityActorInfo();

};