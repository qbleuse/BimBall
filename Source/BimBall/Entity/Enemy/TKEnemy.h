// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TackleEnemy.h"
#include "TKEnemy.generated.h"

/**
 * Enemy That can alternate between tackle and basic attacks
 */
UCLASS()
class BIMBALL_API ATKEnemy : public ATackleEnemy
{
	GENERATED_BODY()

#pragma region ENEMY_BEHAVIOR
		/** ENEMY_BEHAVIOR */

	protected:
		/* gives if the enemy can tackle or not*/
		UPROPERTY(BlueprintReadWrite, Category = EnemyTackle, meta = (AllowPrivateAcess = "true"))
		bool canTackle = true;

		/* delay before enemy can tackle again after a tackle */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EnemyTackle, meta = (AllowPrivateAcess = "true"))
		float canTackleCoolDown = 4.0f;

		/* handle to control if it can do tackles or not */
		FTimerHandle tackleHandle;

	protected:
		virtual bool TryAttack(class ABimBallCharacter* playerChar_)override;
		virtual void DisableAttack_Implementation()override;

		UFUNCTION(BlueprintNativeEvent)
		void ResetCanTackle();
		virtual void ResetCanTackle_Implementation();

	/** END ENEMY_BEHAVIOR */
#pragma endregion
	
};
