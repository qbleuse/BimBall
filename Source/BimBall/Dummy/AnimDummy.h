// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AnimDummy.generated.h"

/**
 * 
 */
UCLASS()
class BIMBALL_API UAnimDummy : public UAnimInstance
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(BlueprintReadWrite)
	bool isUp = true;

	UPROPERTY(BlueprintReadWrite)
	float downRotation = 0;

public:
	UFUNCTION()
	void OnUp();

	UFUNCTION()
	void OnDown(bool downOnPositiveSide);
};
