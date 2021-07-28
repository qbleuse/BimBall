// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BimBall/Skills/SkillWithCooldown.h"
#include "SkillWithCooldownUtilities.generated.h"

/**
 * 
 */
UCLASS()
class BIMBALL_API USkillWithCooldownUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	/* Is not on cooldown */
	UFUNCTION(BlueprintCallable, Category = "FSkillWithCooldown")
	static bool IsReady(const FSkillWithCooldown& skill, float currentTime)
	{
		return skill.lastUsedTime + skill.cooldown <= currentTime;
	}

	/* Is subclass valid */
	UFUNCTION(BlueprintCallable, Category = "FSkillWithCooldown")
	static bool IsSkillValid(const FSkillWithCooldown& skill)
	{
		return skill.subClass.Get() != nullptr;
	}
};
