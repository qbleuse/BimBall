// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "DoIHaveBall.generated.h"

/**
 * Buffer to call the AEnemyController EnemyHaveBal,
 * wich determines if the Enemy is carying the ball,
 * from The Behavior Tree.
 */
UCLASS()
class BIMBALL_API UDoIHaveBall : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()
	
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
