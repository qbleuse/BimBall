// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "Components/SphereComponent.h"
#include "BimBall/Skills/StaggerData.h"
#include "BimBall/BallHittable.h"
#include "Components/TimelineComponent.h"
#include "Entity.generated.h"

class ABall;

UCLASS()
class BIMBALL_API AEntity : public ACharacter, public IBallHittable
{
	GENERATED_BODY()

	/* Stats */
private:
	UPROPERTY(EditAnywhere, Category = "Stats")
	float hp = 5;



protected:
	/* time since creation */
	float lastStaggerTime = 0;
	float staggerDuration = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Stats")
	float hpMax = 5;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Stats")
	float hpRegenPerSec = 0.3;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Stats")
	float atk = 1;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Stats")
	UAnimMontage* hitAnim = nullptr;

protected:
	UFUNCTION(BlueprintCallable)
	void AddHp(float addedHp);

	UFUNCTION(BlueprintCallable)
	float GetHp() const;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetStaggerDuration() const { return staggerDuration; }

public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsAlive() const
	{
		return hp > 0;
	}

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsDead() const
	{
		return !IsAlive();
	}

	UFUNCTION(BlueprintCallable)
	float GetHpRatio() const;

	UFUNCTION(BlueprintCallable)
	virtual float Stagger(const FStaggerData& staggerData);

	UFUNCTION(BlueprintCallable)
	bool IsStaggered() const;

	/* End Stats */


#pragma region BALL_INTERACTION
	/** BALL_INTERACTION */

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName ballAttachedSocketName = "ballHandler";

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TWeakObjectPtr<ABall> ball = nullptr;

	/* Distance at which the entity automatically snaps to the ball */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USphereComponent* ballRecoveryRange = nullptr;

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ReceiveBall(ABall* b);
	virtual void ReceiveBall_Implementation(ABall* ball);

	UFUNCTION(BlueprintCallable)
	bool IsBallNear(const ABall* testedBall) const;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE ABall* GetBall() const
	{
		return ball.Get();
	}

	/** END BALL_INTERACTION */
#pragma endregion 

public:
	// Sets default values for this character's properties
	AEntity();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


#pragma region SNAP
	/** DAMAGES */

	protected:
		UPROPERTY()
		FTimeline snapTimeline;

		UPROPERTY(EditAnywhere)
		UCurveFloat* snapCurve = nullptr;

		UPROPERTY()
		FVector locBeforeSnap;
		UPROPERTY()
		FVector snapTargetLoc;

	protected:
		UFUNCTION(BlueprintCallable)
		void OnSnapTick();

	public:
		UFUNCTION(BlueprintCallable)
		void SnapToActor(AActor* snapTarget);

		UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void SnapToLocation(const FVector& snapLocation);
		virtual void SnapToLocation_Implementation(const FVector& snapLocation);

	/** END SNAP */
#pragma endregion 

#pragma region DAMAGES
	/** DAMAGES */

	protected:
		// Called when the entity has no hp left
		UFUNCTION(BlueprintNativeEvent)
		void OnHpZero(AActor* killer);
		virtual void OnHpZero_Implementation(AActor* killer);

	public:
		UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void DealDamages(AActor* attacker, float rawDamages);
		virtual void DealDamages_Implementation(AActor* attacker, float rawDamages);

		/* Ignore Damages Modifiers */
		UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void DealRawDamages(AActor* attacker, float damages);
		virtual void DealRawDamages_Implementation(AActor* attacker, float damages);

		//UFUNCTION(BlueprintNativeEvent)
		//void OnBallHit(ABall* hitBall);
		//virtual void OnBallHit_Implementation(ABall* hitBall);
		virtual void OnBallHit_Implementation(ABall* hitBall) override;

		UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		float RawToFinalDamages(float rawDamages) const;
		virtual float RawToFinalDamages_Implementation(float rawDamages) const;

	/** END DAMAGES */
#pragma endregion 

#pragma region AIM
	/** AIM */

public:
	// Heavy
	AActor* GetAutoAimTarget(const TArray<AActor*>& potentialTargets, const TFunction<float(const AActor& actor)>& aimScoreGetter) const;

	/** END AIM */
#pragma endregion 

};
