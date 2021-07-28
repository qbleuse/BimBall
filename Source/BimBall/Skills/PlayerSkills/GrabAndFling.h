// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BimBall/Skills/Skill.h"
#include "BimBall/Skills/StaggerData.h"
#include "GrabAndFling.generated.h"

/**
 * 
 */
UCLASS()
class BIMBALL_API UGrabAndFling : public USkill
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float knockbackImpulse = 5000;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float upKnockbackImpulse = 5000;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName attachmentBone = "ballHandler";

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float maxDistanceToAttack = 800;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FScoreAutoAim aimData;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FStaggerData staggerDataOnHit;


public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TWeakObjectPtr<AActor> target = nullptr;

public:
	virtual void Start_Implementation() override;
	virtual void Tick_Implementation(float deltaTime) override;
	virtual void End_Implementation() override;

	float GetAutoAimScore(const AActor& aimTarget) const;

	virtual bool IsStoppable() const override
	{
		return false;
	}
};
