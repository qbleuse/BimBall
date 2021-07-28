// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BaseEnemyAnimation.generated.h"

/* Enum listing different run stats. */
UENUM(BlueprintType)
enum class EEnemyRunState : uint8
{
	RUN				UMETA(DisplayName = "Run"), /* When the player does nothing or runs */
	KAMIKAZE_RUN	UMETA(DisplayName = "Kamikaze Run"),
	TACKLER_RUN		UMETA(DisplayName = "Tackler Run"),
};

/**
 * 
 */
UCLASS()
class BIMBALL_API UBaseEnemyAnimation : public UAnimInstance
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EEnemyRunState runState = EEnemyRunState::RUN;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool isDead = false;

	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	void SetRun();

	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	void SetKZRun();

	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	void SetTKRun();
};
