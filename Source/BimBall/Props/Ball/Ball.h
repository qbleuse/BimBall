// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BimBall/Props/Ball/BallChainOptions.h"
#include "BimBall/Skills/StaggerData.h"
#include "Ball.generated.h"

class AEntity;
class ABimBallCharacter;

UENUM(BlueprintType)
enum class EBallState : uint8
{
	/* When the player carries the ball around. */
	IS_CARRIED UMETA(DisplayName = "Is carried"),
	/* When the ball is thrown upon an enemy. */
	IS_THROWN UMETA(DisplayName = "Is targeting"),
	/* When the ball returns to the thrower. */
	IS_RETURNING UMETA(DisplayName = "Is returning"),
	/* When the ball is dropped, has not target and is not carried. */
	IS_DROPPED UMETA(DisplayName = "Is dropped")
};

UCLASS()
class BIMBALL_API ABall : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	UStaticMeshComponent* ballMesh = nullptr;

	/* 
	 * Velocity of the ball when thrown. 
	 * Is not valid when it is dropped.
	 */
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	FVector velocity = FVector::ZeroVector;
	/*
	 * Current Target of the throw.
	 * If nullptr, there is currently no target.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TWeakObjectPtr<AActor> target = nullptr;
	/*
	 * Current owner of the ball,
	 * meaning the player has the ball in its hands 
	 * or that the ball must return to the owner after a throw.
	 * If nullptr, the ball has no owner and is dropped.
	 * If the ball was thrown and the owner is nullptr, 
	 * then the ball goes in a random direction and is dropped.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TWeakObjectPtr<AEntity> ownerCharacter = nullptr;
	/* Speed of the ball when thrown. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float speed = 10;
	/* Distance at which the ball "hits" the target. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float targetHitDist = 100;

	/* Damages dealt to entities when thrown upon them. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float damages = 1.f;

	/*
	 * After being thrown or dropped, the ball can't be taken for a short time.
	 * This is the handler to set back isTakable.
	 */
	UPROPERTY()
	FTimerHandle isTakableHandler;
	/* 
	 * After being thrown or dropped, the ball can't be taken for a short time.
	 * This variable is false at this moment, true overrise. 
	 */
	UPROPERTY()
	bool isTakable = true;
	/* duration for which the ball can't be taken after a drop */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float dropUntakableDuration = 1.f;

	/* Rebound */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float reboundUp = 45;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float reboundScale = 1000;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float reboundAngle = 45;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float reboundFollowVelocityPower = 0.3;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool shouldReboundBeOpposite = false;

	/* Drop */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float dropRandomHalfAngleInDegrees = 30;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float dropImpulseLength = 1000;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool returnToOwnerAfterHit = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBallChainOptions chainOptions;

public:
	/* Stagger dealt to the ball target. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FStaggerData hitStaggerData;

public:	
	/* Sets default values for this actor's properties */
	ABall();

protected:
	/* Called when the game starts or when spawned */
	virtual void BeginPlay() override;

	/* Called when the ball hits a target */
	UFUNCTION(BlueprintNativeEvent)
	void OnTargetHit();
	virtual void OnTargetHit_Implementation();

public:	
	/* Called when the ball is thrown at a target. */
	UFUNCTION(BlueprintNativeEvent)
	void OnThrown(AEntity* thrower, AActor* newTarget, bool isThrowCharged);
	virtual void OnThrown_Implementation(AEntity* thrower, AActor* newTarget, bool isThrowCharged);

	/* Called when the ball is dropped in a direction. */
	UFUNCTION(BlueprintNativeEvent)
	void OnDropped(FVector impulse = FVector::ZeroVector);
	virtual void OnDropped_Implementation(FVector impulse = FVector::ZeroVector);

	/* Called when an entity receives the ball. */
	UFUNCTION(BlueprintNativeEvent)
	void OnReceived(AEntity* newOwner);
	virtual void OnReceived_Implementation(AEntity* newOwner);

	/* Getter */
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetDamages() const
	{
		return damages;
	}

	/* Utility accessor */
	UFUNCTION(BlueprintCallable)
	FORCEINLINE AEntity* GetOwnerCharacter() const
	{
		return ownerCharacter.Get();
	}

	/* Returns true if the ball is charged (when thrown) */
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsCharged() { return (chainOptions.nbReboundsLeft >= 1); }

	/* Get the state of the ball */
	UFUNCTION(BlueprintCallable)
	EBallState GetState() const;

	/* Called every frame */
	virtual void Tick(float DeltaTime) override;

	/* 
	 * Returns true if an entity can take the ball 
	 * (if it is takable and if another entity doesn't already have it).
	 */
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetIsTakable() const
	{
		return isTakable && !ownerCharacter.IsValid();
	}

	/*
	 * Returns true if the ball owner has the ball in its hands.
	 */
	UFUNCTION(BlueprintCallable)
	bool IsInOwnerHands() const;

	/*
	 * Returns true if the ball is hidden by an obstacle from viewLocation.
	 */
	UFUNCTION(BlueprintCallable)
	bool IsBallOutOfView(const FVector& viewLocation) const;

	/*
	 * Returns true if the ball is hidden by an obstacle from the 0th player's camera.
	 */
	UFUNCTION(BlueprintCallable)
	bool IsBallOutOfCameraView() const;

#pragma region INDICATORS
	/** INDICATORS */

protected:
	UPROPERTY(BlueprintReadWrite)
	AActor* ballSeeThroughIndicator = nullptr;

	/*
	 * Distance between the player and the ball from which the player should see the ball though the walls.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float seeThroughDistanceTreshold = 3000.0;

	/* Can we see the ball through walls ? */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	bool isSeeThrough = false;

	/* Half size of the indicator texture, to fix the default offset */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector2D ballIndicatorBounds = { 35, 50.0 };

	/* Half size of the screen, to fix the offset */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector2D ballIndicatorOffset = { 960, 540 };

protected:
	/* Compute the location of the ball in screen coordinates */
	UFUNCTION(BlueprintCallable)
	bool ComputeBallScreenLocation(FVector2D& outputLoc) const;

	/* Compute the location of the ball indicator in screen coordinates (from the ball screen coordinates) */
	UFUNCTION(BlueprintCallable)
	FVector2D ComputeBallIndicatorLocation(const FVector2D& targetScreenLoc) const;

	UFUNCTION(BlueprintCallable)
	FVector2D ComputeBallIndicatorBounds() const;

	/* Compute the angle of the ball indicator depending on its coordinates. */
	UFUNCTION(BlueprintCallable)
	float ComputeBallIndicatorAngle(const FVector2D& ballIndicatorLocation) const;

	/* Sets the location of the ball ground indicator with a raycast to the ground. */
	UFUNCTION(BlueprintCallable)
	void UpdateGroundIndicatorLocation(class USceneComponent* ballGroundIndicator, ECollisionChannel traceChannel);

	/* 
	 * Update the location of the ground indicator 
	 * and hides it if an entity has it in its hands. 
	 */
	UFUNCTION(BlueprintCallable)
	void UpdateGroundIndicator(class USceneComponent* ballGroundIndicator, ECollisionChannel traceChannel);

	/*
	 * Calls EnableBallSeeThrough() if the ball is hidden by something, DisableBallSeeThrough() overrise.
	 */
	UFUNCTION(BlueprintCallable)
	void UpdateBallSeeThrough();

	/*
	 * Sets the material and properties to see the ball through walls.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void EnableBallSeeThrough();
	virtual void EnableBallSeeThrough_Implementation() {}

	/*
	 * Removes the material and properties to see the ball through walls.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void DisableBallSeeThrough();
	virtual void DisableBallSeeThrough_Implementation() {}

	/** END INDICATORS */
#pragma endregion 
};
