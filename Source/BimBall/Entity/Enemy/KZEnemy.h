// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TackleEnemy.h"
#include "KZEnemy.generated.h"

/**
 * An Enemy That Explodes when close to the player
 */
UCLASS()
class BIMBALL_API AKZEnemy : public ATackleEnemy
{
	GENERATED_BODY()

#pragma region ENEMY_BEHAVIOR
		/** ENEMY_BEHAVIOR */

protected:
	/* boolean to know if the move of the Kamikaze is a tackle or the BaseEnemy*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = KamikazeProperty, meta = (AllowPrivateAccess = "true"))
	bool useTackleBehavior = false;

	/* callback spawned after explosion of the kamikaze */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = OnDeath, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> onDeathSpawnedActor = nullptr;

protected:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Explode();
	void Explode_Implementation();

	virtual void Attack_Implementation(class ABimBallCharacter* playerChar_)override;
	virtual void OnHpZero_Implementation(AActor* killer)override;

public:
	virtual bool TryAttack(class ABimBallCharacter* playerChar_)override;

	virtual void OnBallHit_Implementation(ABall* hitBall) override;

	/** END ENEMY_BEHAVIOR */
#pragma endregion
	
};
