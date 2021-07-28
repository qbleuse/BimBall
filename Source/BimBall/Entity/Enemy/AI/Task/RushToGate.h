// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "RushToGate.generated.h"

/**
 * Buffer to call the AEnemyController MoveToGate,
 * which Move Enemy to TouchDown in a Yard,
 * from the Behavior Tree.
 */
UCLASS()
class BIMBALL_API URushToGate : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)override;

};
