// Fill out your copyright notice in the Description page of Project Settings.


#include "IsNotEngaged.h"
#include "BimBall/Entity/Enemy/AI/EnemyController.h"

bool UIsNotEngaged::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AEnemyController* enemyAI = Cast<AEnemyController>(OwnerComp.GetAIOwner());

	/* Guards */
	if (!enemyAI)
		return false;

	return !enemyAI->IsEngaged();
}

