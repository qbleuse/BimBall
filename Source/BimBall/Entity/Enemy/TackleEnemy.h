// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEnemy.h"
#include "TackleEnemy.generated.h"

/**
 * An Enemy that tackle for attack
 */
UCLASS()
class BIMBALL_API ATackleEnemy : public ABaseEnemy
{
	GENERATED_BODY()

#pragma region ATACKLEENEMY
		/** ATACKLEENEMY */

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	ATackleEnemy();

	virtual float Stagger(const FStaggerData& staggerData)override;

	/** END ATACKLEENEMY */
#pragma endregion

#pragma region TACKLE_INFO
		/** TACKLE_INFO */

	protected:
		/* stagger data applied to player when stagger work */
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = EnemyStatus, meta = (AllowPrivateAcess = "true"))
		FStaggerData tackleStaggerData;

		/* Decision boolean of Enemy*/
		UPROPERTY(BlueprintReadWrite, Category = EnemyTackle, meta = (AllowPrivateAcess = "true"))
		bool isTackling = false;

		/* Tackle Spd of Enemy*/
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = EnemyTackle, meta = (AllowPrivateAcess = "true"))
		float tackleSpd = 300.0f;

		/* Tackle Dist of Enemy*/
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = EnemyTackle, meta = (AllowPrivateAcess = "true"))
		float tackleDist = 500.0f;

		/* Tackle Damage given to the player on Impact */
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = EnemyTackle, meta = (AllowPrivateAcess = "true"))
		float tackleDmg = 2.0f;

		/* Tackle Delay Before Initiation */
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = EnemyTackle, meta = (AllowPrivateAcess = "true"))
		float tackleDelay = 3.0f;

		/* Tackle CoolDown After Finished */
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = EnemyTackle, meta = (AllowPrivateAcess = "true"))
		float tackleCooldown = 5.0f;

		/* Tackle Dur Before Forced Finish */
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = EnemyTackle, meta = (AllowPrivateAcess = "true"))
		float tackleMaxDur = 10.0f;

		/* Tackle's strength of steering */
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = EnemyTackle, meta = (AllowPrivateAcess = "true"))
		float steeringStrength = 10.0f;

		/* Dir of the Tackle */
		UPROPERTY(BlueprintReadWrite, Category = EnemyTackle, meta = (AllowPrivateAcess = "true"))
		FVector tackleDir = FVector::ZeroVector;

		/* Sphere HitBox Around Enemy */
		UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = EnemyHitBox, meta = (AllowPrivateAcess = "true"))
		class USphereComponent* tackleHitZone = nullptr;

		/* Radius Of Sphere HitBox Around Enemy */
		UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = EnemyHitBox, meta = (AllowPrivateAcess = "true"))
		float tackleHitZoneRadius = 150.0f;

	public:
		FORCEINLINE float GetTackleSqrdDist	()const { return tackleDist * tackleDist; }
		FORCEINLINE void SetTackleSpd		()		{ if (charaMvt) { charaMvt->MaxWalkSpeed = tackleSpd; } }

	/** END TACKLE_INFO */
#pragma endregion

#pragma region ENEMY_BEHAVIOR
	/** ENEMY_BEHAVIOR */

	protected:
		UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		bool Tackle(class ABimBallCharacter* playerChar_);
		virtual bool Tackle_Implementation(class ABimBallCharacter* playerChar_);

		UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void StartTackle();
		virtual void StartTackle_Implementation();

		virtual void Attack_Implementation(class ABimBallCharacter* playerChar_)override;

	public:
		virtual void OnBallHit_Implementation(ABall* hitBall) override;

		virtual bool TryAttack(class ABimBallCharacter* playerChar_)override;
		virtual void DisableAttack_Implementation()override;

	/** END ENEMY_BEHAVIOR */
#pragma endregion

};
