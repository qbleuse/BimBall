// Fill out your copyright notice in the Description page of Project Settings.


#include "CamProfileManager.h"
#include "BimBall/GameManagement/BimBallGameMode.h"
#include "BimBall/Entity/Player/BimBallCharacter.h"
#include "BimBall/Entity/Enemy/EnemyManager.h"
#include "FocusCamProfile.h"
#include "GameFramework/Controller.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "CameraProfile.h"
#include "Camera/CameraShake.h"
#include "TimerManager.h"

/**================================ ACAMPROFILEMANAGER ================================*/

// Sets default values
ACamProfileManager::ACamProfileManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACamProfileManager::BeginPlay()
{
	Super::BeginPlay();

	ABimBallGameMode* gameMode = Cast<ABimBallGameMode>(GetWorld()->GetAuthGameMode());
	if (gameMode && gameMode->GetEnemyManager())
		enemyManager = gameMode->GetEnemyManager();
	
	/* Set Cam Profile */
	if (basicCamProfile)
		camProfile = NewObject<UCameraProfile>(this, *basicCamProfile);

	/* Set Dist to disable special cam profile */
	if (ballCamProfile)
	{
		UFocusCamProfile* focusProfile = NewObject<UFocusCamProfile>(this, *ballCamProfile);
		if (focusProfile)
			maxFocusDist = focusProfile->GetMaxDistFromTarget();
	}
}

// Called every frame
void ACamProfileManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/* can't change profile if ball cam activated */
	if (!isBallCamActivated)
	{
		/* enable fight profile automaticly if player engaged */
		if (!isFightCamActivated && IsPlayerEngaged())
		{
			SetCamProfile(fightCamProfile);
			isFightCamActivated = true;
		}/* else return to normal cam */
		else if (isFightCamActivated && !IsPlayerEngaged())
		{
			SetCamProfile(basicCamProfile);
			isFightCamActivated = false;
		}

	}/* disable BallCam if dist from cam > maxFocusDist */
	else if (playerChar && enemyManager && enemyManager->GetBall() &&
		(playerChar->GetActorLocation() - enemyManager->GetBall()->GetActorLocation()).Size() >= maxFocusDist)
	{
		BallCamActivate();
	}

	/* update camProfile*/
	camProfile->TickUpdate(DeltaTime);

	/* Hide Character, if charactre too close */
	if (playerChar && camProfile &&
		(playerChar->GetFollowCamera()->GetComponentLocation() - playerChar->GetActorLocation()).SizeSquared() <= (camProfile->playerDisapearanceDist * camProfile->playerDisapearanceDist))
		playerChar->SetActorHiddenInGame(true);
	else if (playerChar)
		playerChar->SetActorHiddenInGame(false);
	
}

/**================================ CAMERA_COMPONENT ================================*/

float ACamProfileManager::GetHeight(float pitch)const
{
	if (!FollowCamera)
		return pitch;

	/*				/| -> cam
	 *			   / |
	 *armLength <-/	 |
	 *			 /	 |
	 *			/	 |
	 *		   /	 |
	 *        /		 | -> sin(360.0f - pitch) * armLength
	 *	     /		 |
	 *	    /		 |
	 *	   /		 |
	 *	  /			 |
	 *	 /)->(360.0f - pitch)
	 *	/------------|
	 *  ^
	 *  |
	 *  player
	 */

	float armLength = (FollowCamera->GetComponentLocation() - playerChar->GetActorLocation()).Size();
	float height	= armLength * FMath::Sin(FMath::DegreesToRadians(360.0f - pitch));
	return  height;
}

/**================================ ENTITIES_REF ================================*/

void ACamProfileManager::SetPlayer(class ABimBallCharacter* playerChar_)
{
	/* Set Ref */
	if (playerChar_ && playerChar_->GetCameraBoom() && playerChar_->GetFollowCamera())
	{
		playerChar		= playerChar_;
		CameraBoom		= playerChar->GetCameraBoom();
		FollowCamera	= playerChar->GetFollowCamera();
	}

	/* setup cam Profile */
	if (camProfile && playerChar)
	{
		camProfile->Init(this);
	}
}

/**================================ CAMERA_PROFILE ================================*/

void ACamProfileManager::SetCamProfile(TSubclassOf<class UCameraProfile> aCamProfile)
{
	/* Set a Cam Profile, with function called in the right order */
	if (aCamProfile)
	{
		camProfile->Destroy();		
		camProfile = NewObject<UCameraProfile>(this, *aCamProfile);
		camProfile->Init(this);
	}
}

/**================================ CAMERA_INPUT ================================*/

void ACamProfileManager::BallCamActivate()
{
	/* change ballCam profile, by the basic one */
	if (isBallCamActivated)
	{
		SetCamProfile(basicCamProfile);
		isBallCamActivated = false;
		return;
	}

	/* set the ball cam, if dist from cam <= maxFocusDist */
	if (playerChar && enemyManager && enemyManager->GetBall() &&
		(playerChar->GetActorLocation() - enemyManager->GetBall()->GetActorLocation()).Size() <= maxFocusDist)
	{
		SetCamProfile(ballCamProfile);
		UFocusCamProfile* focusCamProfile = Cast<UFocusCamProfile>(camProfile);

		if (focusCamProfile)
		{
			focusCamProfile->target = enemyManager->GetBall();
			maxFocusDist			= focusCamProfile->GetMaxDrawBack();
			isBallCamActivated		= true;
		}
	}
}

void ACamProfileManager::SetYaw(float yaw)const
{
	/* Guards */
	if (!playerChar)
		return;

	playerChar->Controller->SetControlRotation(FRotator(playerChar->GetControlRotation().Pitch, yaw, 0.0f));
}

void ACamProfileManager::SetPitch(float pitch)const
{
	/* Guards */
	if (!playerChar)
		return;

	playerChar->Controller->SetControlRotation(FRotator(pitch, playerChar->GetControlRotation().Yaw, 0.0f));
}

void ACamProfileManager::LookUpAtRate_Implementation(float Rate)
{
	/* An Input of The player Remove Ball Cam */
	if (isBallCamActivated && FMath::Abs(Rate) >= camInputSensibility)
	{
		SetCamProfile(basicCamProfile);
		isBallCamActivated = false;
	}/* Guards */
	if (playerChar)
	{
		float pitch = playerChar->GetControlRotation().Pitch;
		pitch		= camProfile->LookUpAtRate(pitch, Rate);
		SetPitch(pitch);
	}
}

void ACamProfileManager::TurnAtRate_Implementation(float Rate)
{
	/* An Input of The player Remove Ball Cam */
	if (isBallCamActivated && FMath::Abs(Rate) >= camInputSensibility)
	{
		SetCamProfile(basicCamProfile);
		isBallCamActivated = false;
	}/* Guards */
	if (playerChar)
	{
		float yaw	= playerChar->GetControlRotation().Yaw;
		yaw			= camProfile->TurnAtRate(yaw, Rate);
		SetYaw(yaw);
	}
}

void ACamProfileManager::LookUp_Implementation(float Rate)
{
	/* An Input of The player Remove Ball Cam */
	if (isBallCamActivated && FMath::Abs(Rate) >= camInputSensibility)
	{
		SetCamProfile(basicCamProfile);
		isBallCamActivated = false;
	}/* Guards */
	if (playerChar)
	{
		float pitch = playerChar->GetControlRotation().Pitch;
		pitch		= camProfile->LookUp(pitch, Rate);
		SetPitch(pitch);
	}
}

void ACamProfileManager::Turn_Implementation(float Rate)
{
	/* An Input of The player Remove Ball Cam */
	if (isBallCamActivated && FMath::Abs(Rate) >= camInputSensibility)
	{
		SetCamProfile(basicCamProfile);
		isBallCamActivated = false;
	}/* Guards */
	if (playerChar)
	{
		float yaw	= playerChar->GetControlRotation().Yaw;
		yaw			= camProfile->Turn(yaw, Rate);
		SetYaw(yaw);
	}
}

FVector ACamProfileManager::GetFrwdVec(float Value) 
{
	/* Guards */
	if (playerChar)
	{
		return camProfile->GetFrwdVec(playerChar->GetControlRotation().Yaw, Value);
	}

	return FVector::ZeroVector;
}

FVector ACamProfileManager::GetRightVec(float Value) 
{
	/* Guards */
	if (playerChar)
	{
		return camProfile->GetRightVec(playerChar->GetControlRotation().Yaw, Value);
	}

	return FVector::ZeroVector;
}

void ACamProfileManager::ResetCam(float wantedYaw, float wantedPitch)
{
	/* Guards */
	if (camProfile)
	{
		camProfile->Reset(wantedYaw, wantedPitch);
	}
}

/**================================ CAMERA_EFFECT ================================*/

void ACamProfileManager::Shake_Implementation(TSubclassOf<UCameraShake> shakeClass, float duration)
{
	/* Guards */
	if (playerChar)
	{
		APlayerController* playerController = Cast<APlayerController>(playerChar->AIControllerClass);
		if (!playerController)
			return;

		UCameraShake* camerashake = playerController->PlayerCameraManager->PlayCameraShake(shakeClass);

		if (duration >= 0.0f)
			GetWorldTimerManager().SetTimer(shakeHandle, FTimerDelegate::CreateUObject(this,&ACamProfileManager::StopShake,camerashake),duration, false);
	}
}

void ACamProfileManager::StopShake_Implementation(UCameraShake* cameraShake)
{
	/* Guards */
	if (playerChar)
	{
		APlayerController* playerController = Cast<APlayerController>(playerChar->AIControllerClass);
		if (!playerController)
			return;

		playerController->PlayerCameraManager->StopCameraShake(cameraShake);
	}
}

void ACamProfileManager::StopAllShake()
{
	/* Guards */
	if (playerChar)
	{
		APlayerController* playerController = Cast<APlayerController>(playerChar->AIControllerClass);
		if (!playerController)
			return;

		playerController->PlayerCameraManager->StopAllCameraShakes();
	}
}