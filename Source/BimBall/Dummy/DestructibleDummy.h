// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BimBall/Dummy/BaseDummy.h"
#include "DestructibleDummy.generated.h"

/**
 * Dummy that can be destructed.
 */
UCLASS()
class BIMBALL_API ADestructibleDummy : public ABaseDummy
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float life = 5;

public:
	virtual void OnBallHit_Implementation(class ABall* hitBall) override;

	virtual void DealDamages(float rawDamages);
};
