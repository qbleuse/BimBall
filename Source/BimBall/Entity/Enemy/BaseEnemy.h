// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BimBall/Entity/Entity.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BaseEnemy.generated.h"

/**
 * Base Class of Enemy, basic implementation of Enemy actions. 
 */
UCLASS()
class BIMBALL_API ABaseEnemy : public AEntity
{
	GENERATED_BODY()
	
#pragma region ABASEENEMY
	/** ABASEENEMY */

	protected:
		/* struct to save Stagger Data on wall hit */
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = EnemyStatus, meta = (AllowPrivateAcess = "true"))
		FStaggerData staggerOnHitData;

		/* boolean to know if stagger is applied on wall hit  */
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = EnemyStatus, meta = (AllowPrivateAcess = "true"))
		bool shouldStaggerOnHit = false;

		/* 0 = there is no wall, 1 = even the floor and the ceil are walls */
		static constexpr float wallTreshold = 0.2;

		virtual float Stagger(const FStaggerData& staggerData)override;

	protected:
		// Called when the game starts or when spawned
		virtual void BeginPlay() override;

		/* method called when enemy hit a wall (stagger only if shouldStaggerOnHit == true)*/
		UFUNCTION()
		void OnWallHit(UPrimitiveComponent*		HitComp, 
						AActor*					OtherActor, 
						UPrimitiveComponent*	OtherComp, 
						FVector					NormalImpulse, 
						const FHitResult&		Hit);

		virtual void OnHpZero_Implementation(AActor* killer) override;

		/* called in deal damage to compute final damage dealed by enemy */
		virtual float RawToFinalDamages_Implementation(float rawDamages) const override;

	public:
		ABaseEnemy();

		/* apply Stagger, used in WallHit */
		FORCEINLINE void StaggerOnHit(const FStaggerData& newStaggerOnHitData)
		{
			staggerOnHitData = newStaggerOnHitData;
			shouldStaggerOnHit = true;
		}

	/** END ABASEENEMY */
#pragma endregion

#pragma region PLAYER_INTERACTION
	/** PLAYER_INTERACTION */

	protected:
		/* struct to save Stagger Data on enemy being attacked */
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = EnemyStatus, meta = (AllowPrivateAcess = "true"))
		FStaggerData atkStaggerData;

		/* boolean to save the state of vulnerability of enemy */
		UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = EnemyStatus, meta = (AllowPrivateAcess = "true"))
		bool isVulnerable = false;

		/* Vulnerabily State Duration */
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = EnemyStatus, meta = (AllowPrivateAcess = "true"))
		float vlnrablityDur = 1.0f;

		/*  Vulnerabily Damage Taken Multiplier */
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = EnemyStatus, meta = (AllowPrivateAcess = "true"))
		float vlnrablityAtkMultiplier = 1.0f;

		/* Handle to control the vulnerability status of enemy */
		FTimerHandle statusHandle;

		/* Sphere HitBox Around Enemy */
		UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = EnemyHitBox, meta = (AllowPrivateAcess = "true"))
		class USphereComponent* hitZone = nullptr;

		/* Radius Of Sphere HitBox Around Enemy */
		UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = EnemyHitBox, meta = (AllowPrivateAcess = "true"))
		float hitZoneRadius = 150.0f;

	protected:
		/* set vulnerable to false */
		UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void ResetVulnerable();
		void ResetVulnerable_Implementation();

		/* set vulnerable to true, reset to false after vlnrablityDur*/
		void SetVulnerable_Implementation();

		/* method to apply stagger on being attacked */
		UFUNCTION(BlueprintCallable)
		void ApplyStagger(class ABimBallCharacter* playerChar, FStaggerData& stagger);

	public:
		virtual void OnBallHit_Implementation(ABall* hitBall) override;

		UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void SetVulnerable();

		UFUNCTION(BlueprintCallable)
		FORCEINLINE bool IsVulnrble()const { return isVulnerable; }

	/** END PLAYER_INTERACTION */
#pragma endregion

#pragma region ENEMY_CALLBACKS
	/** ENEMY_CALLBACKS */

	public:
		/* method used in BPs */
		UFUNCTION(BlueprintCallable)
		float GetControllerVibrationFromHit(float hitDamage, float minVibration, float maxVibration);

	/** END ENEMY_CALLBACKS */
#pragma endregion

#pragma region ENEMY_STATUS
	/** ENEMY_STATUS */

	protected:
		UCharacterMovementComponent* charaMvt = nullptr;

		/* Base Spd of Enemy*/
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = EnemySpeed, meta = (AllowPrivateAcess = "true"))
		float runSpd			= 300.0f;

		/* Ball Holding Spd of Enemy*/
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = EnemySpeed, meta = (AllowPrivateAcess = "true"))
		float ballHoldingSpd	= 400.0f;
		
		/* trying to Catch Ball Spd of Enemy*/
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = EnemySpeed, meta = (AllowPrivateAcess = "true"))
		float catchSpd			= 350.0f;

		/* Atking player Spd of Enemy*/
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = EnemySpeed, meta = (AllowPrivateAcess = "true"))
		float atkingSpd			= 350.0f;

		/* Blocking player Spd of Enemy*/
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = EnemySpeed, meta = (AllowPrivateAcess = "true"))
		float blockSpd			= 320.0f;

	public:

		FORCEINLINE void SetRunSpd()		{ if (charaMvt) { charaMvt->MaxWalkSpeed = runSpd; } }
		FORCEINLINE void SetBallHoldingSpd(){ if (charaMvt) { charaMvt->MaxWalkSpeed = ballHoldingSpd; } }
		FORCEINLINE void SetCatchSpd()		{ if (charaMvt) { charaMvt->MaxWalkSpeed = catchSpd; } }
		FORCEINLINE void SetAtkingSpd()		{ if (charaMvt) { charaMvt->MaxWalkSpeed = atkingSpd; } }
		FORCEINLINE void SetBlockSpd()		{ if (charaMvt) { charaMvt->MaxWalkSpeed = blockSpd; } }

	/** END ENEMY_STATUS */
#pragma endregion

#pragma region ENEMY_BEHAVIOR
	/** ENEMY_BEHAVIOR */

	protected:
		/* Dist which Enemy will start chasing Player */
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = EnemyBehavior, meta = (AllowPrivateAcess = "true"))
		float chaseDist = 900.0f;
	
		/* Dist which Enemy will surround Player */
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = EnemyBehavior, meta = (AllowPrivateAcess = "true"))
		float surroundDist = 500.0f;
	
		/* Dist which Enemy will try to pick up dropped ball */
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = EnemyBehavior, meta = (AllowPrivateAcess = "true"))
		float ballPickUpDist = 800.0f;

		/* Dist which Enemy will draw back of the player afeter atk */
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = EnemyBehavior, meta = (AllowPrivateAcess = "true"))
		float runAwayDist = 400.0f;

		/* Dist which Enemy will block the player between pos of the enemy that carries the ball and player */
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = EnemyBehavior, meta = (AllowPrivateAcess = "true", UIMin = "0.0", UIMax = "1.0"))
		float blockRatio = 0.5f;
	
		/* cooldown before Enemy can attack */
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = EnemyBehavior, meta = (AllowPrivateAcess = "true"))
		float atkCooldown = 2.0f;

		/* cooldown before Enemy can attack */
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = EnemyBehavior, meta = (AllowPrivateAcess = "true"))
		float atkDelay	= 0.2f;
	
		/* Handle to the attack of the enemy */
		FTimerHandle atkHandle;
	
		/* boolean used in attack to start it or mke it in pause */
		UPROPERTY(BlueprintReadWrite, Category = EnemyBehavior, meta = (AllowPrivateAcess = "true"))
		bool canAttack = true;

	public:
		/* Location of nearest touchDown Location */
		UPROPERTY(BlueprintReadWrite, Category = EnemyMovement, meta = (AllowPrivateAccess = "true"))
		FVector touchDownLoc = FVector::ZeroVector;

		/* Location of nearest touchDown Location */
		UPROPERTY(BlueprintReadWrite, Category = EnemyMovement, meta = (AllowPrivateAccess = "true"))
		FVector blockLoc = FVector::ZeroVector;


		/* indices used in manager */
		UPROPERTY(BlueprintReadWrite, Category = EnemyBehavior, meta = (AllowPrivateAcess = "true"))
		int32 managerIndex = -1;

		UPROPERTY(BlueprintReadWrite, Category = EnemyBehavior, meta = (AllowPrivateAcess = "true"))
		int32 engageIndex = -1;

		UPROPERTY(BlueprintReadWrite, Category = EnemyBehavior, meta = (AllowPrivateAcess = "true"))
		int32 atkingIndex = -1;

	protected:
		/* handle to make callbacks in BPS */
		FTimerHandle spriteHandle;
		FTimerHandle animHandle;

		/* delay for sprite callback */
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = EnemyBehavior, meta = (AllowPrivateAcess = "true"))
		float spriteDelay = 0.0f;

		/* delay for anim callback */
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = EnemyBehavior, meta = (AllowPrivateAcess = "true"))
		float animDelay = 0.0f;

		/* callback event */
		UFUNCTION(BlueprintImplementableEvent)
		void SpriteEvent();

		UFUNCTION(BlueprintImplementableEvent)
		void AnimEvent();

		/* Event called in try attack to call attack function with a delay */
		UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void StartAttack(class ABimBallCharacter* playerChar_);
		virtual void StartAttack_Implementation(class ABimBallCharacter* playerChar_);

		/* Event called when attack is done */
		UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void Attack(class ABimBallCharacter* playerChar_);
		virtual void Attack_Implementation(class ABimBallCharacter* playerChar_);
		
		/* Event called after a delay to set canAttack to true */
		UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void ResetCanAttack();
		virtual void ResetCanAttack_Implementation();
	
	public:
	
		FORCEINLINE float	GetSqrdChaseDist		()const	{ return chaseDist * chaseDist; }
		FORCEINLINE float	GetSqrdSurroundDist		()const { return surroundDist * surroundDist; }
		FORCEINLINE float	GetSqrdBallPickUpDist	()const { return ballPickUpDist * ballPickUpDist; }
		FORCEINLINE float	GetSqrdRunAwayDist		()const { return runAwayDist * runAwayDist; }
		FORCEINLINE float	GetSurroundDist			()const { return surroundDist; }
		FORCEINLINE float	GetRunAwayDist			()const	{ return runAwayDist; }
		FORCEINLINE float	GetChaseDist			()const { return chaseDist; }
		FORCEINLINE float	GetBlockRatio			()const { return blockRatio; }
		UFUNCTION(BlueprintCallable)
		FORCEINLINE bool	GetCanAttack			()const { return canAttack; }
		UFUNCTION(BlueprintCallable)
		FORCEINLINE bool	HasBall					()const { return (ball != nullptr); }
	
		
		/* called each frame when enemy attacking */
		virtual bool TryAttack(class ABimBallCharacter* playerChar_);

		/* Event created because put TRyAttack in UFUNCTION resulted in a stack overflow by unreal */
		UFUNCTION(BlueprintImplementableEvent)
		void TryAttackEvent();

		/* called each frame when trying to get the ball */
		UFUNCTION(BlueprintNativeEvent)
		bool TryPickUp(class ABall* ball_);
		virtual bool TryPickUp_Implementation(class ABall* ball_);

		/* called after attack finished */
		UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void DisableAttack();
		virtual void DisableAttack_Implementation();


	/** END ENEMY_BEHAVIOR */
#pragma endregion

};
