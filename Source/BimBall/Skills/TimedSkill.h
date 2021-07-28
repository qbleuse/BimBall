// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BimBall/Skills/Skill.h"
#include "TimedSkill.generated.h"

/**
 * Skill used when the skill has a specific duration in time.
 * The skill automacally ends after duration time.
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class BIMBALL_API UTimedSkill : public USkill
{
	GENERATED_BODY()
	
protected:
	/* The time the skill starts at in game time seconds. */
	float startTime;

	/* The duration of the skill in seconds. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float duration = 0.7;

public:
	virtual void Start_Implementation() override;
	virtual void Tick_Implementation(float deltaTime) override;
};
