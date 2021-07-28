// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "IsStaggered.generated.h"

/**
 * Buffer to call the AEnemyController IsStaggered,
 * wich determines if the Enemy is in staggered state,
 * from The Behavior Tree.
 */
UCLASS()
class BIMBALL_API UIsStaggered : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	
};
