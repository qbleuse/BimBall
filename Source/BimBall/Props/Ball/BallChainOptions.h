// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GenericPlatform/GenericPlatform.h"
#include "BallChainOptions.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FBallChainOptions
{
	GENERATED_BODY()

	/* The maximum number of rebounds possible. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 nbReboundsMax = 3;
	/* The amount of rebounds remaining. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int32 nbReboundsLeft = 0;

	/* Enemies already hit, used to not hit them again. */
	UPROPERTY()
	TArray<AActor*> alreadyHitEnemies;

	/* Maximum distance the must be between the ball and the next rebound target. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float reboundRadius = 400;
	
	/* Resets the ball, depending on wether the ball is "charged" or not. */
	void Reset(bool isCharged);

	/* Returns true if a rebound has been consumed. */
	bool Consume();

	/* Returns the next target of the rebound. */
	class AActor* FindNextTarget(class ABall* ball);

};