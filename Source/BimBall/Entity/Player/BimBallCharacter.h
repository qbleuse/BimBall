// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BimBall/Props/Ball/Ball.h"
#include "BimBall/Entity/Entity.h"
#include "BimBall/Skills/Skill.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "Components/SphereComponent.h"
#include "BimBall/Skills/ScoreAutoAim.h"
#include "BimBall/Skills/SkillWithCooldown.h"
#include "BimBallCharacter.generated.h"

/* Movement types of the player */
UENUM()
enum class ECharacterMovementType : uint8
{
	OUT_OF_FIGHT 	UMETA(DisplayName = "Out Of Fight"), /* When the player does nothing or runs */
	IN_FIGHT		UMETA(DisplayName = "Fighting"),
	AIMING			UMETA(DisplayName = "Aiming"),
	TAKING_BALL 	UMETA(DisplayName = "TakingBall"),
	DEAD			UMETA(DisplayName = "Dead"),
};

UCLASS(config=Game)
class ABimBallCharacter : public AEntity
{
	GENERATED_BODY()

#pragma region ABIMBALLCHARACTER
	/** ABIMBALLCHARACTER */

	protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	public:
		ABimBallCharacter();

	/** END ABIMBALLCHARACTER */
#pragma endregion 

#pragma region STATS
	/* Stats */
private:
	UPROPERTY(EditAnywhere, Category = "Stats")
	float rage = 1;

	/* Last time the player has been damaged in game time. */
	float lastDamageTime = -99999;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	float healthRegenerationDelay = 3;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Stats")
	float rageMax = 5;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Stats")
	float rageIncreasePerSec = 0.1;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Stats")
	float rageDecreasePerSec = 0.2;

protected:
	UFUNCTION(BlueprintCallable)
	bool IsRegenerating() const;

	UFUNCTION(BlueprintCallable)
	void AddRage(float addedRage);

	UFUNCTION(BlueprintCallable)
	float GetRage() const;

public:
	UFUNCTION(BlueprintCallable)
	bool HasBall() const;

	UFUNCTION(BlueprintCallable)
	float GetRageRatio() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnRageFull();
	virtual void OnRageFull_Implementation() {}

	virtual float RawToFinalDamages_Implementation(float rawDamages) const override;

	/* End Stats */
#pragma endregion

#pragma region BALL_INTERACTION
	/** BALL_INTERACTION */

	private:
		UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
		bool isAiming = false;

		UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
		USphereComponent* ballSnapRange = nullptr;

		/* When Aiming Only */
		UPROPERTY()
		bool isChargedAtAim = false;

	protected:
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Subclasses")
		TSubclassOf<ABall> ballSubclass;

		UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FScoreAutoAim aimData;

		UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float currentBallChargeDuration = 0;

		UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float ballChargeDuration = 1;

		/* Time in seconds that an entity should be not visible before the player will not be enable to aim at it, (one frame is 1.1 for imitate an epsilon) */
		UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (UIMin = "0.0"))
		float disableAimTime = 0.1f;

		/* angle of cone in front of player wich enemy will be aimed at (please enter the half of the angle desired for the cone.) */
		UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (UIMin = "0.0", UIMax = "180.0"))
		float aimLimitAngle = 45.f;

		UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TSubclassOf<AActor> selectionUISubClass;

		UPROPERTY(BlueprintReadWrite, EditAnywhere)
		AActor* selectionUIInstance = nullptr;

	protected:
		UFUNCTION()
		void OnBallEnterDetectionRange(class UPrimitiveComponent* OverlappedComp,
										class AActor* OtherActor,
										class UPrimitiveComponent* OtherComp,
										int32 OtherBodyIndex,
										bool bFromSweep,
										const FHitResult& SweepResult);

	public:
		UFUNCTION()
		void SetIsAiming(bool newValue);

		UFUNCTION()
		FORCEINLINE bool GetIsAiming() const
		{
			return isAiming;
		}

		UFUNCTION(BlueprintCallable)
		FORCEINLINE bool IsAimingWithChargedBall() const
		{
			return currentBallChargeDuration >= ballChargeDuration;
		}

		/* Called when the ball is finally charged (called once). */
		UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void OnBallCharged();
		virtual void OnBallCharged_Implementation() {}

		/* Called when the ball is not charged anymore (called once). */
		UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void OnBallUncharged();
		virtual void OnBallUncharged_Implementation() {}

		/* Called when the player starts to aim (called once). */
		UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void OnAimStart();
		virtual void OnAimStart_Implementation() {}

		/* Called when the player ends aiming (called once). */
		UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void OnAimEnd();
		virtual void OnAimEnd_Implementation() {}

		UFUNCTION(BlueprintCallable)
		float GetBallAutoAimScore(const AActor* ballTarget) const;

		UFUNCTION(BlueprintCallable)
		AActor* GetBallAutoAimTarget() const;

		virtual void ReceiveBall_Implementation(ABall* ball) override;

		FORCEINLINE class ABall* GetBall()const { return ball.Get(); }

	/** END BALL_INTERACTION */
#pragma endregion 

#pragma region STATE
	/** STATE */

	protected:
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subclasses")
		TSubclassOf<USkill> chargeSubclass;
		UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		USkill* chargeSkill = nullptr;

		/* The skill currently used. */
		UPROPERTY(BlueprintReadWrite)
		USkill* currentSkill = nullptr;

		/* The next skill that might be used. */
		//UPROPERTY(BlueprintReadWrite)
		//TSubclassOf<USkill> nextSkill = nullptr;
		FSkillWithCooldown* nextSkill = nullptr;

		UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		bool isEnraged = false;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EnemyInteraction, meta = (AllowPrivateAccess = "true"))
		float playerEngagedDist = 1000.0f;

		FTimerHandle enragedHandle;
		FTimerHandle incapacitatedHandle;

		/* Duration for which the player stays on the ground after losing the ball. */
		UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		float incapacitationDuration = 2.f;

		UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float enragedCooldownAfterDroppingBall = 3.f;

	public:
		void SetEnraged(bool newIsEnragedValue);

		FORCEINLINE bool IsEnraged() const
		{
			return isEnraged;
		}

	/** END STATE */
#pragma endregion 

#pragma region ATK
	/** ATK */

	protected:
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float chargeSpeed = 800.0f;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Subclasses")
		FSkillWithCooldown spinSub;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subclasses")
		FSkillWithCooldown basicAttackSub;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subclasses")
		FSkillWithCooldown ballRushSub;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subclasses")
		FSkillWithCooldown grabAndFlingSub;

		UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FVector2D attackDirection = {0, 0};

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ball", meta = (AllowPrivateAccess = "true"))
		FVector dropDir = FVector::ZeroVector;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ball", meta = (AllowPrivateAccess = "true"))
		float dropImpulse = 0.0f;

	public:
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FVector2D joystickDirection = { 0, 0 };

	protected:

		virtual void DropBall_Implementation();
		virtual void Aim_Implementation();
		virtual void Throw_Implementation();
		virtual void Spin_Implementation();
		virtual bool TryBasicAttack_Implementation();
		virtual void GrabEntity_Implementation(AEntity* grabbedEntity);
		virtual void FlingEntity_Implementation();



		UFUNCTION(BlueprintCallable)
		bool TryUseSkill(FSkillWithCooldown& skillSubClass);

	public:
		UFUNCTION(BlueprintCallable)
		void GrabEntityAuto();

		void CatchNearBall();

		UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void DropBall();

		/* Called each tick when the player is aiming */
		UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void Aim();

		UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void Throw();

		UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void Spin();

		UFUNCTION(BlueprintCallable)
		void BasicAttack();

		UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		bool TryBasicAttack();

		UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void GrabEntity(AEntity* grabbedEntity);

		UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void FlingEntity();

		UFUNCTION(BlueprintCallable)
		void TryUseNextSkill();

		void SetSpinCooldown(float newCooldown)
		{
			spinSub.cooldown = newCooldown;
		}

		virtual float Stagger(const FStaggerData& staggerData) override;

	/** END ATK */
#pragma endregion 

#pragma region MOVEMENT
	/** MOVEMENT */

	private:
		//UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ECharacterMovementType movementType = ECharacterMovementType::OUT_OF_FIGHT;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		bool isTakingBallSpeedEnabled = false;

		FRotator defaultRotateTurnRate = { 0.f,0.f, 900.f };

	protected:

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float runSpeed = 600.0f;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float fightingSpeed = 300.0f;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float aimingSpeed = 100.0f;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float receivingBallSpeed = 10.0f;

		FTimerHandle coyoteHandle;

		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coyote Time")
		bool coyote = false;

		/* nb of frame that will allow a coyote jump (coyote time basically)*/
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Coyote Time")
		float frameBeforeDisableJump = 10.0f;

	protected:
		UFUNCTION()
		void UpdateMovementType();

		UFUNCTION()
		void SetMovementType(ECharacterMovementType newMovementType);

		virtual void Charge_Implementation();
		virtual void StopCharging_Implementation();

		virtual void UpdateCoyote(float DeltaTime);
		virtual void ResetCoyote();
		virtual void Landed(const FHitResult& Hit)override;

	public:
		/** Called for forwards/backward input */
		void MoveForward(float value);

		/** Called for side to side input */
		void MoveRight(float value);

		UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void Charge();

		UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void StopCharging();


	/** END MOVEMENT */
#pragma endregion 

#pragma region CAMERA
	/** CAMERA */

	protected:
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<class ACamProfileManager> subCamManager = nullptr;

		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class ACamProfileManager* camManager = nullptr;

		/** Camera boom positioning the camera behind the character */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom = nullptr;

		/** Follow camera */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera = nullptr;

	public:
		/* Reset cam on the line passing through the player and cam target */
		UFUNCTION(BlueprintCallable)
		void ResetCamToActor(AActor* camTarget);

		/* will reset Cam behind the closest actor on the line passing through both of them*/
		UFUNCTION(BlueprintCallable)
		void ResetCamToClosestActor(AActor* actor1, AActor* actor2);

		void ResetCamProfile();

		void SetupCamManager(class UInputComponent* playerInputComponent);

		/** Returns CameraBoom subobject **/
		FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

		/** Returns FollowCamera subobject **/
		FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

		bool IsPlayerEngaged()const;

	/** END CAMERA */
#pragma endregion 

#pragma region DAMAGES
	/** DAMAGES */

		virtual void DealDamages_Implementation(AActor* attacker, float rawDamages) override;

		virtual void OnHpZero_Implementation(AActor* killer) override;

	/** END DAMAGES */
#pragma endregion 

};

