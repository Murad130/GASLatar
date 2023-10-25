// Copyright Latar

#pragma once

#include "CoreMinimal.h"
#include "Character/BorshCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "EnemyCharacter.generated.h"


/**
 * 
 */
UCLASS()
class GASLATAR_API AEnemyCharacter : public ABorshCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()
		
public:
	AEnemyCharacter();
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
	
};
