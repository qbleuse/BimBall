// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StaggerData.generated.h"

/**
 * Data used when dealing the Stagger Effect.
 */
USTRUCT(BlueprintType)
struct FStaggerData
{
	GENERATED_BODY()

	/*========= Default Values =========*/

	/* knockback direction of the stagger effect */
	FVector knockbackDir = FVector::ZeroVector; 
	/* knockback distance of the stagger effect */
	float knockbackDistance = 100.f; 
	/* duration of the stagger effect (e.g. the duration the entity will be incapacitated)*/
	float duration = 1.f; 

	/*========= Modifiable values =========*/

	/* Knockback distance multiplier : a higher value means a greater knockback. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float knockbackDistanceMultiplier = 1.f;

	/* The minimum knockback distance possible */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float minDistance = 0.f;

	/* The maximum knockback distance possible */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float maxDistance = 100000.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float knockbackAngle = 20;

	/* Duration the entity is incapacited. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float durationMultiplier = 1.f;

	/* The minimum duration the stagger effect can last. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float minDuration = 0.f;

	/* The maximumm duration the stagger effect can last. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float maxDuration = 10.f;

	/* boolean to know if the stagger is force applied 
	 * to an entity that would not have taken it otherwise */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool forceStagger = false;
};