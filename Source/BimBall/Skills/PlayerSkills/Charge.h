// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BimBall/Skills/Skill.h"
#include "BimBall/Skills/StaggerData.h"
#include "Charge.generated.h"

/**
 * 
 */
UCLASS()
class BIMBALL_API UCharge : public USkill
{
	GENERATED_BODY()

protected:
	float previousSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float chargeSpeed = 800;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float chargeDmgRadius = 200;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float maxDamages = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float minDamages = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FStaggerData staggerData;

public:
	bool shouldKnockbackEnemies = false;

public:
	virtual void Start_Implementation() override;
	virtual void Tick_Implementation(float deltaTime) override;
	virtual void End_Implementation() override;
};
