// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ScoreAutoAim.generated.h"

/**
 * Struct used to compute score uniformly.
 * Other parameters can be used to compute the score,
 * but these parameters are often used, so they are packed together.
 */

USTRUCT(BlueprintType)
struct FScoreAutoAim
{
	GENERATED_BODY()

	/*
	 * Higher distScoreScale is,
	 * better is the importance of the distance 
	 * to compute the score.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float distScoreScale = 100.f;

	/*
	 * Higher distScoreScale is,
	 * better is the importance of the angle
	 * to compute the score.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float inFrontScoreScale = 1.f;
	
	/* 
	 * If the distance between the skill owner and the tested target
	 * is higher than maxDist, then the target is not selected.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float maxDist = 2000.f;
};