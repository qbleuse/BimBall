// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BlockPlayer.generated.h"

/**
 * Buffer to call the AEnemyController MoveToBlock,
 * which Move Enemy to block player and prevent him to catch up the Enemy Carying the ball,
 * from the Behavior Tree.
 */
UCLASS()
class BIMBALL_API UBlockPlayer : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)override;

};
