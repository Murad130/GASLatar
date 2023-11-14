// Copyright Latar

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BorshGameModeBase.generated.h"

class UCharacterClassInfo;

/**
 * 
 */
UCLASS()
class GASLATAR_API ABorshGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults")
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;



};
