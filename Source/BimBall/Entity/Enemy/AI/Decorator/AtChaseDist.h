// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "AtChaseDist.generated.h"

/**
 * Buffer to call the AEnemyController AtChaseDist,
 * wich determines if the Enemy is at a sufficient dist from the player
 * to chase him determined in the Enemy, from the Behavior Tree.
 */
UCLASS()
class BIMBALL_API UAtChaseDist : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
