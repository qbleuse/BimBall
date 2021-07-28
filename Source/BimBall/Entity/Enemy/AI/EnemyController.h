// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BimBall/Entity/Enemy/BaseEnemy.h"
#include "BimBall/Entity/Enemy/EnemyManager.h"
#include "EnemyController.generated.h"

/**
 * AIController of Enemy, All Task and Decorator will call method inside of it 
 */
UCLASS()
class BIMBALL_API AEnemyController : public AAIController
{
	GENERATED_BODY()

#pragma region AENEMYCONTROLLER
		/** AENEMYCONTROLLER */

	protected:
		virtual void OnPossess(APawn* InPawn) override;
		virtual void OnUnPossess() override; 

		/** END AENEMYCONTROLLER */
#pragma endregion

#pragma region MOVEMENT
/** MOVEMENT */

	protected:
		/* radius around the location that Enemy should go where the movement would be considered completed */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EnemyMovement, meta = (AllowPrivateAccess = "true"))
		float moveToAcceptanceRadius = 150.0f;

		/* radius around the Player that Enemy should go where the movement would be considered completed */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EnemyMovement, meta = (AllowPrivateAccess = "true"))
		float toPlayerAcceptanceRadius = 150.0f;

	public:
		/* Return true when Enemy is at moveToAcceptanceRadius of the point given in parameter, or if ctrdEnemy == nullptr.
		 * Stop Movement in the first case, do not move in the second. */
		UFUNCTION(BlueprintCallable)
		bool GetTo(FVector point, float acceptanceRadius);

		/* Call GetTo with Player's Location*/
		UFUNCTION(BlueprintCallable)
		bool GetToPlayer();

		/* Call GetTo with Ball's Location*/
		UFUNCTION(BlueprintCallable)
		bool GetToBall();

	/** END MOVEMENT */
#pragma endregion

#pragma region ENEMY
		/** ENEMY */

	protected:
		/* Enemy controlled by EnemyController */
		UPROPERTY(BlueprintReadOnly)
		ABaseEnemy* ctrldEnemy = nullptr;

		/* Manager of Enemy */
		UPROPERTY(BlueprintReadOnly)
		UEnemyManager* enemyManager = nullptr;

	public:
		FORCEINLINE ABaseEnemy*			GetEnemy()const				{ return ctrldEnemy; }

		/* boolean to know if enemy has Ball Steal */
		UFUNCTION(BlueprintCallable)
		FORCEINLINE bool				DidEnemiesStoleBall()const	{ return enemyManager ? enemyManager->ballBeingCarried : false; }

		/* method to update context of enemy in manager */
		FORCEINLINE void				UnEngageEnemy()const		{ if (enemyManager) { enemyManager->RemoveEngageEnemy(ctrldEnemy); } }
		FORCEINLINE void				RmAtkingEnemy()const		{ if (enemyManager) { enemyManager->RemoveAtkingEnemies(ctrldEnemy); } }

	/** END ENEMY */
#pragma endregion

#pragma region DECORATOR
		/** DECORATOR */
	protected:
		/* Compute dist of Enemy from TargetLoc, and look if it is under MaxDist*/
		UFUNCTION(BlueprintCallable)
		bool AtDist(FVector targetLoc, float maxDist)const;

	public:
		/* Call AtDist with Player as target and ChaseDist as MaxDist*/
		bool AtChaseDist()const;
		/* Call AtDist with Ball as target and ballPickUpDist as MaxDist*/
		bool AtBallPickUpDist()const;

		/* determines if enemy is able to attack player,
		 * ask the manager, if it is allowed attack, if able to do so*/
		UFUNCTION(BlueprintCallable)
		bool CanAttack()const;

		/* determines if ball is on the ground */
		UFUNCTION(BlueprintCallable)
		bool IsBallDropped()const;

		/* look if our enemy is staggered*/
		UFUNCTION(BlueprintCallable)
		FORCEINLINE bool IsStaggered()const		{ return (ctrldEnemy ?		ctrldEnemy->IsStaggered()		: false); }

		/* look if our enemy is engaged with the player */
		UFUNCTION(BlueprintCallable)
		FORCEINLINE bool IsEngaged()const		{ return (ctrldEnemy ?		(ctrldEnemy->engageIndex >= 0) : false); }

		/* ask the manager if an enemy has the ball */
		UFUNCTION(BlueprintCallable)
		FORCEINLINE bool EnemiesHaveBall()const { return (enemyManager ?	enemyManager->EnemyHasBall()	: false);}

		/* look if our enemy has the ball */
		UFUNCTION(BlueprintCallable)
		FORCEINLINE bool EnemyHaveBall()const	{ return (ctrldEnemy ?		ctrldEnemy->HasBall()			: false); }

	/** END DECORATOR */
#pragma endregion

#pragma region TASK
	/** DECORATOR */
	
	public:
		/* Base Task, Try to engage the player, and if it is try to surrond player to attack player */
		void MoveToSurround();

		/* Called if an enemy has the ball, all enemies aren't attacking or engaged with the player,
		 * close enough of the player, will move to try to block the player */
		void MoveToBlock();

		/* Called if this enemy has the ball, will try to move to the closest yard */
		void MoveToGate();

		/* InProgress Task, returns true when attack has been performed or not able to attack anymore*/
		bool TryAttack();

		/* InProgress Task, returns true when ball being picked up */
		bool TryPickUpBall();

		/* get all yard thanks to enemyManager and returns the location of the closest yard */
		FVector GetClosestYard(FVector enemyLoc);
	
	/** END DECORATOR */
#pragma endregion
	
};
