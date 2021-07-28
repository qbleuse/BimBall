// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "IsNotEngaged.generated.h"

/**
 * Buffer to call the AEnemyController IsEngaged,
 * wich determines if the Enemy is engaged with player, and takes the inverse,
 * from The Behavior Tree.
 */
UCLASS()
class BIMBALL_API UIsNotEngaged : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
