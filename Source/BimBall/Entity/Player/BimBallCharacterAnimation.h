// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "BimBallCharacterAnimation.generated.h"

/* Different states */
UENUM()
enum class EBimBallAnimState : uint8
{
	IDLE			UMETA(DisplayName = "Idle"), /* When the player does nothing or runs */
	THROW			UMETA(DisplayName = "Throw"),
	AIM				UMETA(DisplayName = "Aim"),
	GRABBING		UMETA(DisplayName = "Grabbing"),
	INCAPACITATED	UMETA(DisplayName = "Incapacitated"),
	TAKINGBALL		UMETA(DisplayName = "TakingBall"),
};

/**
 * 
 */
UCLASS()
class BIMBALL_API UBimBallCharacterAnimation : public UAnimInstance
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EBimBallAnimState animState;

private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool isBallHigherThanPlayer = false;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class ABimBallCharacter* pawnOwner = nullptr;

public:
	virtual void NativeBeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void UpdateBallRelativeLoc(const ABall* ball);

	UFUNCTION(BlueprintCallable)
	void SetIdle()
	{
		animState = EBimBallAnimState::IDLE;
	}

	UFUNCTION(BlueprintCallable)
	void SetIncapacitated()
	{
		animState = EBimBallAnimState::INCAPACITATED;
	}

	UFUNCTION(BlueprintCallable)
	void StopIncapacitated()
	{
		animState = EBimBallAnimState::INCAPACITATED;
	}

	UFUNCTION(BlueprintCallable)
	void SetTakingBall()
	{
		animState = EBimBallAnimState::TAKINGBALL;
	}

	UFUNCTION(BlueprintCallable)
	void RunAimAnim();

	UFUNCTION(BlueprintCallable)
	void RunThrowAnim();

	UFUNCTION(BlueprintCallable)
	void RunGrabAnim();
	
	UFUNCTION(BlueprintCallable)
	void RunFlingAnim();

	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	FORCEINLINE bool IsIdle() const
	{
		return animState == EBimBallAnimState::IDLE;
	}

	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	FORCEINLINE bool IsAiming() const
	{
		return animState == EBimBallAnimState::AIM;
	}

	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	FORCEINLINE bool IsInAimOrThrowState() const
	{
		return animState == EBimBallAnimState::AIM || animState == EBimBallAnimState::THROW;
	}

	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	FORCEINLINE bool IsThrowing() const
	{
		return animState == EBimBallAnimState::THROW;
	}

	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	FORCEINLINE bool IsGrabbing() const
	{
		return animState == EBimBallAnimState::GRABBING;
	}

	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	FORCEINLINE bool IsIncapacitated() const
	{
		return animState == EBimBallAnimState::INCAPACITATED;
	}

	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	FORCEINLINE bool IsTakingBall() const
	{
		return animState == EBimBallAnimState::TAKINGBALL;
	}
};
