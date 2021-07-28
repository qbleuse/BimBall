// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "Attack.generated.h"

/**
 * Buffer to call the AEnemyController TryAttack,
 * wich executes the attack behavior of the enemy, 
 * from the Behavior Tree.
 * The Task returns InProgress until TryAttack return true,
 * wich means Enemy Attacked.
 */
UCLASS()
class BIMBALL_API UAttack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult);
	virtual void EndTask(UBehaviorTreeComponent& OwnerComp, EBTNodeResult::Type TaskResult);
};
