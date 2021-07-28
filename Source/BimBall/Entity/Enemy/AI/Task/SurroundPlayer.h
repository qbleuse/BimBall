// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "SurroundPlayer.generated.h"

/**
 * Buffer to call the AEnemyController MoveToSurround,
 * which Move Enemy to surround player to be prepared to attack,
 * from the Behavior Tree.
 */
UCLASS()
class BIMBALL_API USurroundPlayer : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)override;
};
