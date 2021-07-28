// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "Stagger.generated.h"

/**
 * The Task returns InProgress until IsStaggered return false,
 * wich means isn't staggerered anymore.
 */
UCLASS()
class BIMBALL_API UStagger : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult);
	virtual void EndTask(UBehaviorTreeComponent& OwnerComp, EBTNodeResult::Type TaskResult);

};
