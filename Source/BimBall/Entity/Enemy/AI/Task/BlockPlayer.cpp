// Fill out your copyright notice in the Description page of Project Settings.


#include "BlockPlayer.h"
#include "BimBall/Entity/Enemy/AI/EnemyController.h"

EBTNodeResult::Type UBlockPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AEnemyController* enemyAI = Cast<AEnemyController>(OwnerComp.GetAIOwner());

	/* Guards */
	if (!enemyAI)
		return EBTNodeResult::Aborted;

	enemyAI->MoveToBlock();

	return EBTNodeResult::Succeeded;
}