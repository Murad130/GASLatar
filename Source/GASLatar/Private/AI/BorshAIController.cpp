// Copyright Latar


#include "AI/BorshAIController.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

ABorshAIController::ABorshAIController()
{
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>("BlackboardComponent");
	check(Blackboard);
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>("BehaviorTreeComponent");
	check(BehaviorTreeComponent);
}
