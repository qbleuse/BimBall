// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BimBall/Skills/Skill.h"
#include "Templates/SubclassOf.h"
#include "Engine/World.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SkillWithCooldown.generated.h"

/**
 * The class is NOT a skill.
 * This class is used to have a cooldown between each skill invocation.
 */
USTRUCT(BlueprintType)
struct FSkillWithCooldown 
{
	GENERATED_BODY();

public:
	/* The skill that will be generated. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<USkill> subClass;

	/* The last time this generator was used to invocate the skill. */
	float lastUsedTime = 0.0f;

	/* Cooldown between each use of the skill. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float cooldown = 0.0f;

	/* Is not on cooldown */
	bool IsReady(float currentTime) const
	{
		return lastUsedTime + cooldown <= currentTime;
	}

	/* Is subclass valid */
	bool IsSkillValid() const
	{
		return subClass.Get() != nullptr;
	}

	/* Utility Accessor, returns the UClass */
	UClass* Get() const
	{
		return subClass.Get();
	}

	/* Utility Accessor, returns the UClass */
	UClass* operator*() 
	{
		return *subClass;
	}
};
