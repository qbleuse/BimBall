// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BimBall/Skills/Skill.h"
#include "BallRush.generated.h"

/**
 * Ball Rush is the action of the entity snapping/dashing to the ball (to catch it).
 * The skill currently ends just after it starts (and not after the snap).
 */
UCLASS()
class BIMBALL_API UBallRush : public USkill
{
	GENERATED_BODY()
	
public:
	/* The maximum distance the ball must be to snap to it */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float ballSnapRangeRadius = 400;

public:
	virtual void Start_Implementation() override;

};
