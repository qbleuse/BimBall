// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BimBall/Skills/TimedSkill.h"
#include "BimBall/Skills/StaggerData.h"
#include "Spin.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class BIMBALL_API USpin : public UTimedSkill
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float spinRadius = 500;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float maxDamages = 2;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float minDamages = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float spinShorterCooldown = 3;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float spinDefaultCooldown = 7;

	/* stagger effect applied when the entity hit the wall*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FStaggerData staggerDataOnHit;

	/* stagger applied when the entity is hit with the spin */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FStaggerData stagger;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UAnimMontage* anim = nullptr;

	UFUNCTION(BlueprintCallable)
	void DealDamages();

	virtual void Start_Implementation() override;

	virtual bool IsStoppable() const override
	{
		return false;
	}

	virtual float ApplyDamagesModifiers_Implementation(float rawDamages) const override;
};
