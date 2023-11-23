// Copyright Latar

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BorshAIController.generated.h"

class UBlackboardComponent;
class UBehaviorTreeComponent;

/**
 * 
 */
UCLASS()
class GASLATAR_API ABorshAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	ABorshAIController();

protected:

	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;
};
