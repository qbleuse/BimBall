// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EngineUtils.h"
#include "EnemyManager.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class BIMBALL_API UEnemyManager : public UObject
{
	GENERATED_BODY()

#pragma region PLAYER/BALL
		/** PLAYER/BALL */

	protected:
		UPROPERTY(BlueprintReadOnly)
		class ABimBallGameMode* gameMode = nullptr;

		UPROPERTY(BlueprintReadOnly)
		class ABimBallCharacter* playerChar = nullptr;

		UPROPERTY(BlueprintReadOnly)
		class ABall* ball = nullptr;

	public:

		FORCEINLINE class ABimBallCharacter*	GetPlayerChar()const	{ return playerChar; }
		FORCEINLINE class ABall*				GetBall()const			{ return ball; }
		FORCEINLINE class ABimBallGameMode*		GetGameMode()const		{ return gameMode; }

		void Set(class ABimBallGameMode* gameMode, class ABimBallCharacter* playerChar_);

		/** END PLAYER/BALL */
#pragma endregion 

#pragma region ENEMIES_MANAGE
	/** ENEMIES_MANAGE */

	protected:
		/* array of all living enemies for avoid use of statics*/
		UPROPERTY(BlueprintReadOnly)
		TArray<class ABaseEnemy*> enemies;
	
		/* array of all enemies close enough of player */
		UPROPERTY(BlueprintReadOnly)
		TArray<class ABaseEnemy*> engagedEnemies;

		/* array of all enemies attacking */
		UPROPERTY(BlueprintReadOnly)
		TArray<class ABaseEnemy*> atkingEnemies;
	
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EnemyBehavior, meta = (AllowPrivateAccess = "true", UIMin = "0", UIMax = "12"))
		int32 maxEngagedEnemiesNb = 12;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EnemyBehavior, meta = (AllowPrivateAccess = "true", UIMin = "0", UIMax = "12"))
		int32 maxAtkingEnemiesNb = 3;

		/* use to do sidestep */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EnemyBehavior, meta = (AllowPrivateAccess = "true", UIMin = "0"))
		float distBetweenEnemies = 150.0f;

	public:
		/* index to get the enemy that has ball */
		UPROPERTY(BlueprintReadOnly)
		int32 ballCarryingEnemy = -2;

		/* variable to know of being ball steal */
		UPROPERTY(BlueprintReadOnly)
		bool ballBeingCarried = false;

	protected:
		void ResetAtkingIndex();
		void ResetEngageIndex();
		void ResetIndex();

	public:
	
		void	AddEnemy			(class ABaseEnemy* newEnemy_);
		void	RemoveEnemy			(class ABaseEnemy* enemy_);
		void	RemoveEngageEnemy	(class ABaseEnemy* enemy_);
		void	RemoveAtkingEnemies	(class ABaseEnemy* enemy_);
		void	RemoveFarestEngageEnemy();
		FVector PreventMoveAnnoyance(class ABaseEnemy*, FVector wantedLocation);

		bool	EngageQuerry(class ABaseEnemy* newEnemy_);
		void	ForceEngage(class ABaseEnemy* newEnemy_);
		bool	AtkQuerry(class ABaseEnemy* newEnemy_);
	
		/* Getter */
		FORCEINLINE int32							GetMaxEngagedEnemiesNb()const		{ return maxEngagedEnemiesNb; }
		FORCEINLINE int32							GetCurrentEngagedEnemiesNb()const	{ return engagedEnemies.Num(); }
		FORCEINLINE const TArray<class ABaseEnemy*>	GetAtkingEnemy()const				{ return atkingEnemies; }
		FORCEINLINE class ABaseEnemy*				GetBallCaryingEnemy()const			{ return ballCarryingEnemy >= 0 ? enemies[ballCarryingEnemy] : nullptr; }
		FORCEINLINE bool							EnemyHasBall()const					{ return (ballCarryingEnemy >= 0); }
		FORCEINLINE const TArray<class ABaseEnemy*> GetEnemies()const					{ return enemies; }
		FORCEINLINE const TArray<class ABaseEnemy*> GetEngageEnemies()const				{ return engagedEnemies; }

	/** END ENEMIES_MANAGE */
#pragma endregion 

};
