// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BimBall/Skills/TimedSkill.h"
#include "BimBall/Skills/StaggerData.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "BasicAttack.generated.h"

/* Data of each attack of the combo.*/
USTRUCT(BlueprintType)
struct FComboStats
{
	GENERATED_BODY()

	/* Dealt damages. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float dmg = 1;
	
	/* Applied stagger. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FStaggerData staggerStats;

	/* Run animation. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UAnimMontage* anim = nullptr;
};

/**
 * 
 */
UCLASS()
class BIMBALL_API UBasicAttack : public UTimedSkill
{
	GENERATED_BODY()
	
protected:
	/* List of the different attacks of the combo. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FComboStats> comboStats;

	/* Current attack index. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 comboIndex = 0;

	/* Distance at which the player doesn't snap anymore. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float tpDistanceFromTarget = 100;

	/* If vulnerable, the max distance at which the target can be from the player. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float maxDistanceToAttackIfVulnerable = 800;

	/* Used to select the target. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FScoreAutoAim aimData;

	/* The last aimed target. */ 
	/* Can be null if there was no target during the last attack. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	AActor* aimTarget = nullptr;

	/* Prevents another attack to be run if true. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	bool isNextAttackReady = true;

	/* Can the current attack be replaced by another skill. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	bool isCurrentlyStoppable = true;


	/* Blendings of run animations. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float blendIn = 0.1;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float blendOut = 0.1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName nextAttackReadyNotifyName = "NextAttackReady";

	/* Notify name of the notify stopping other skills from being available. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName stoppableCancelStartNotifyName = "SpinCancelStart";
	/* Notify name of the notify starting other skills from being available. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName stoppableCancelStopNotifyName = "SpinCancelStop";

public:
	virtual void Start_Implementation() override;

	/* Called when a notify is run. */
	/* Available in blueprints. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnAttackNotify(FName notifyName);
	virtual void OnAttackNotify_Implementation(FName notifyName) {}

	virtual bool IsStoppable() const override
	{
		return isCurrentlyStoppable;
	}

	/* Run when another attack from the combo is used. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void NextCombo();
	virtual void NextCombo_Implementation();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsNextAttackAvailable() const
	{
		return isNextAttackReady;
	}

	/* Run the currently selected attack. */
	void RunCurrentComboAttack();

	/* Reset the combo to the first attack. */
	void ResetCombo();

	float GetAutoAimScore(const AActor& target) const;

	/* Called when a notify is run. */
	/* Callback function added to the OnPlayMontageNotifyBegin delegate. */
	UFUNCTION()
	void OnNotifyAttack(FName notifyName, const FBranchingPointNotifyPayload& branchingPointPayload);
};
