// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "CanAttack.generated.h"

/**
 * Buffer to call the AEnemyController CanAttack,
 * wich determines if the Enemy is allowed to attack the player 
 * determined by the dist from player and the Enemy Manager,
 * from the Behavior Tree.
 */
UCLASS()
class BIMBALL_API UCanAttack : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()
	
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
