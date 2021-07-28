// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "DoEnemiesHaveBall.generated.h"

/**
 * Buffer to call the AEnemyController Enemies Have Ball,
 * wich determines if an ennemy is carrying the ball from the Enemy Manager,
 * from the Behavior Tree.
 */
UCLASS()
class BIMBALL_API UDoEnemiesHaveBall : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
