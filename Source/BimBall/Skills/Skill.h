// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BimBall/Skills/ScoreAutoAim.h"
#include "Skill.generated.h"

class AEntity;

/**
 * Represents a skill that can last in time or not.
 * Has a start, and end, and an owner.
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class BIMBALL_API USkill : public UObject
{
	GENERATED_BODY()

private:
	/* Is Pending Kill */
	bool isFinished = false;

public:
	/* The one who used the skill. */
	/* Must be set before calling Start(). */
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	AEntity* owner = nullptr;

	/* Must be called when the skill must start. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Start();
	virtual void Start_Implementation() {}
	
	/* Must be called when the skill ends. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void End();
	virtual void End_Implementation() 
	{
		isFinished = true;
	}

	/* Called every frame */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Tick(float DeltaTime);
	virtual void Tick_Implementation(float deltaTime) {}

	/* Returns true if this skill can be stopped
	 * and overriden by another skill. 
	 */
	UFUNCTION(BlueprintCallable)
	virtual bool IsStoppable() const
	{
		return true;
	}

	/* Getter of isFinished */
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsFinished() const
	{
		return isFinished;
	}

	/* Modifies the damages got by an entity while using this skill.
	 * Example : Spin returns 0
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	float ApplyDamagesModifiers(float rawDamages) const;
	virtual float ApplyDamagesModifiers_Implementation(float rawDamages) const 
	{ 
		return rawDamages; 
	}

	/* 
	 * Utility function calling the following GetAutoAimScore() 
	 * with the current skill's owner as owner argument
	 */
	float GetAutoAimScore(const AActor& target, const FScoreAutoAim& autoAim) const;
	/*
	 * Utility function calling the following GetAutoAimScore()
	 * with the current owner's forward vector as direction argument
	 */
	static float GetAutoAimScore(const AEntity& owner, const AActor& target, const FScoreAutoAim& autoAim);
	/*
	 * Compute the score of the target 
	 * relative to owner's location, 
	 * the angle difference with the direction vector,
	 * and the auto aim data.
	 */
	static float GetAutoAimScore(const AEntity& owner, const AActor& target, const FScoreAutoAim& autoAim, const FVector& direction);
};
