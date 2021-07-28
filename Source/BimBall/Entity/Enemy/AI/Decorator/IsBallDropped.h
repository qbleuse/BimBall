// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "IsBallDropped.generated.h"

/**
 * Buffer to call the AEnemyController IsBallDropped,
 * wich determines if the ball is on the ground from the Enemy Manager,
 * from The Behavior Tree.
 */
UCLASS()
class BIMBALL_API UIsBallDropped : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()
	
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
