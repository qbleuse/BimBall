// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BallHittable.generated.h"

/**
 * Interface implemented by actors which can be hit by the ball.
 */
UINTERFACE(MinimalAPI, Blueprintable)
class UBallHittable : public UInterface
{
	GENERATED_BODY()


};

class IBallHittable
{
    GENERATED_BODY()

public:
	/* Run when a ball hits the actor. */
	UFUNCTION(BlueprintNativeEvent)
	void OnBallHit(ABall* hitBall);
	virtual void OnBallHit_Implementation(class ABall* hitBall) {}

};