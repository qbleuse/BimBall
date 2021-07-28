// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "AtBallPickUpDist.generated.h"

/**
 * Buffer to call the AEnemyController AtBallPickUpDist,
 * wich determines if the Enemy is at a sufficient dist from the ball
 * determined in the Enemy, from the Behavior Tree.
 */
UCLASS()
class BIMBALL_API UAtBallPickUpDist : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	
};
