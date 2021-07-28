// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BimBallController.generated.h"

/**
 * 
 */
UCLASS()
class BIMBALL_API ABimBallController : public APlayerController
{
	GENERATED_BODY()


	
#pragma region ABIMBALLCONTROLLER
	/** ABIMBALLCONTROLLER */

	protected:
		virtual void BeginPlay()override;
		virtual void SetupInputComponent()override;
	public:
		UFUNCTION(BlueprintCallable)
		void Disable();

		UFUNCTION(BlueprintCallable)
		void Enable();

	/** END ABIMBALLCONTROLLER */
#pragma endregion

#pragma region CHAR_REF
	/** CHAR_REF */

	protected:
	/* ptr on the character it controls */
	UPROPERTY(BlueprintReadWrite)
	class ABimBallCharacter* playerChar = nullptr;

	/** END CHAR_REF */
#pragma endregion

#pragma region SPECIAL_BEHAVIOR
	/** SPECIAL_BEHAVIOR */

	protected:
		FTimerHandle incapitateHandle;

		/* if set to true, all input working wit this controller disabled (everything besides camera and dev)*/
		UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		bool incapacitate = false;

		void IncapacitateFor_Implementation(float duration);
		void EndIncapacitate_Implementation();

	public:
		/* incapacitate for a duration, if <= 0 it last forever*/
		UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void IncapacitateFor(float duration);

		/* end incapacitated state*/
		UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void EndIncapacitate();

	/** END SPECIAL_BEHAVIOR */
#pragma endregion

#pragma region DEV_INPUT
	/** DEV_INPUT */

	protected:
		void ReloadLevel();
		void QuitGame();
	
	/** END DEV_INPUT */
#pragma endregion 

#pragma region ATK_INPUT
	/** ATK_INPUT */

	protected:
		void AimOrCatchBall();
		void CatchNearBall();
		void DropBall();
		void Aim();
		void Throw();
		void Spin();
		void BasicAttack();
		void GrabEntityAuto();
		void FlingEntity();

	/** END ATK_INPUT */
#pragma endregion 

#pragma region MVT_INPUT
	/** MVT_INPUT */

	protected:
		void Jump();
		void StopJumping();
		void MoveForward(float value);
		void MoveRight(float value);
		void Charge();
		void StopCharging();


	/** END MVT_INPUT */
#pragma endregion 

#pragma region CAM_INPUT
	/** CAM_INPUT */

	protected:
		void ResetCamProfile();
	
	/** END CAM_INPUT */
#pragma endregion 


};
