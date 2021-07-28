// Fill out your copyright notice in the Description page of Project Settings.


#include "Stagger.h"
#include "BimBall/Entity/Enemy/AI/EnemyController.h"
#include "BimBall/Entity/Enemy/BaseEnemy.h"

EBTNodeResult::Type UStagger::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	/* Enabling Tick Task */
	bNotifyTick			= 1;
	bNotifyTaskFinished = 0;

	return EBTNodeResult::InProgress;
}

void UStagger::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AEnemyController* enemyAI = Cast<AEnemyController>(OwnerComp.GetAIOwner());

	/* Guards */
	if (!enemyAI)
	{
		EndTask(OwnerComp, EBTNodeResult::Aborted);
		return;
	}

	if (!enemyAI->IsStaggered())
		EndTask(OwnerComp, EBTNodeResult::Succeeded);
}

void UStagger::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	/* Disabling Tick Task */
	bNotifyTick			= 1;
	bNotifyTaskFinished = 0;
}

void UStagger::EndTask(UBehaviorTreeComponent& OwnerComp, EBTNodeResult::Type TaskResult)
{
	/* Disabling Tick Task */
	bNotifyTick			= 0;
	bNotifyTaskFinished = 1;
	FinishLatentTask(OwnerComp, TaskResult);
}