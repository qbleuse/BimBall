// Fill out your copyright notice in the Description page of Project Settings.

#include "ControlCamProfile.h"
#include "CamProfileManager.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"

/**================================ CAMERA_BEHAVIOR ================================*/

void UControlCamProfile::ResetHeight_Implementation()
{
	/* Guards */
	if (camManager)
	{
		/* Prevent a reset of pitch if it would get up just after */
		if (camManager->GetHeight(camInitRot.Pitch) >= heightDistLimit)
			camManager->SetPitch(LookForFloor(camInitRot.Pitch));

		camManager->GetWorldTimerManager().ClearTimer(resetHandle);
	}
}

float UControlCamProfile::LookForFloor(float pitch)const
{
	/* increase the min pitch angle to a value given by GetExtraHeight, to avoid contact with ground */
	float extraHeight				= GetExtraHeight(pitch);
	float lookFloorMinPitchAngle	= FMath::Lerp(minPitchAngle, maxPitchAngle, FMath::Clamp(extraHeight, 0.0f, 1.0f));

	return FMath::Clamp(pitch, maxPitchAngle, lookFloorMinPitchAngle);
}

/**================================ CAM_INPUT ================================*/

float UControlCamProfile::LookUpAtRate(float pitch, float Rate)
{
	/* the pitch will reset if no input is given by the player */
	if (Rate == 0.f)
	{
		camSpeed.Y  = 0.0f;

		if (camManager && !resetHandle.IsValid())
			camManager->GetWorldTimerManager().SetTimer(resetHandle, this, &UControlCamProfile::ResetHeight, resetHeightDelay);

		return LookForFloor(pitch);
	}

	/* if input is given and reset was planned disable reset */
	if (camManager && resetHandle.IsValid())
		camManager->GetWorldTimerManager().ClearTimer(resetHandle);

	/* accel */
	camSpeed.Y	+= Rate * camLookUpAccel * GetWorld()->GetDeltaSeconds();
	camSpeed	= camSpeed.ClampAxes(-camMaxSpeed, camMaxSpeed);

	/* move if it won't make a contact with ground */
	if (camManager && camManager->GetHeight(pitch + (camSpeed.Y * GetWorld()->GetDeltaSeconds())) >= heightDistLimit)
		pitch += (camSpeed.Y * GetWorld()->GetDeltaSeconds());

	return LookForFloor(pitch);
}

float UControlCamProfile::TurnAtRate(float yaw, float Rate)
{
	/* won't move if no input is given */
	if (Rate == 0.f)
	{
		camSpeed.X = 0.0f;
		return yaw;
	}

	/* accel and move */
	camSpeed.X	+= Rate * camTurnAccel * GetWorld()->GetDeltaSeconds();
	camSpeed	= camSpeed.ClampAxes(-camMaxSpeed, camMaxSpeed);
	yaw			+= camSpeed.X * GetWorld()->GetDeltaSeconds();

	/* if a reset is done we want the yaw given to move to stay the same */
	if (!camResetYaw)
		ctrlYaw = yaw;

	return yaw;
}

float UControlCamProfile::LookUp(float pitch, float Rate)
{
	pitch	+= Rate;
	return pitch;
}

float UControlCamProfile::Turn(float yaw, float Rate)
{
	/* if a reset is done we want the yaw given to move to stay the same */
	if (!camResetYaw)
		ctrlYaw = yaw;

	yaw		+= Rate;
	return yaw;
}

FVector  UControlCamProfile::GetFrwdVec(float yaw, float Value)
{
	/* save the position of the joystick */
	ctrlMvt.Y = Value;

	/* the movement is always given by the ctrlYaw */
	return FRotationMatrix(FRotator(0.0,ctrlYaw,0.0f)).GetUnitAxis(EAxis::X);
}

FVector UControlCamProfile::GetRightVec(float yaw, float Value)
{
	/* save the position of the joystick */
	ctrlMvt.X = Value;

	/* the movement is always given by the ctrlYaw */
	return FRotationMatrix(FRotator(0.0,ctrlYaw, 0.0f)).GetUnitAxis(EAxis::Y);
}

void	UControlCamProfile::Reset(float yaw, float pitch)
{
	/* save the info before reset to know when to stop it */
	camResetYaw		= true;
	resetCtrlMvt	= ctrlMvt;

	/* Guards */
	if (camManager)
	{
		camManager->SetYaw(yaw);
		camManager->SetPitch(camInitRot.Pitch);
	}
}

/**================================ GETTER/SETTER ================================*/

void UControlCamProfile::Init(class ACamProfileManager* camManager_)
{
	Super::Init(camManager_);

	/* Guards */
	if (camManager == nullptr || camManager->GetCameraBoom() == nullptr)
		return;

	camManager->SetPitch(camInitRot.Pitch);
	camManager->GetCameraBoom()->bEnableCameraRotationLag	= true;
	camManager->GetCameraBoom()->bEnableCameraLag			= true;
	camManager->GetWorldTimerManager().ClearTimer(resetHandle);
}

void UControlCamProfile::TickUpdate(float deltaTime)
{
	/* Guards */
	if (camManager == nullptr || camManager->GetCameraBoom() == nullptr)
		return;

	camManager->GetCameraBoom()->CameraRotationLagSpeed = FMath::Lerp(camManager->GetCameraBoom()->CameraRotationLagSpeed,camLerpSpeed,deltaTime);
	camManager->GetCameraBoom()->CameraLagSpeed			= FMath::Lerp(camManager->GetCameraBoom()->CameraLagSpeed, camMaxSpeed,deltaTime);

	/* Stop Reset */
	if (FVector2D::DistSquared(ctrlMvt, resetCtrlMvt) >= resetEpsilon)
		camResetYaw = false;
	if (ctrlMvt.Y <= 0.0f)/* DrawBack the cam to a certain dist when player is in front of cam */
		camManager->GetCameraBoom()->TargetArmLength = FMath::Lerp(camManager->GetCameraBoom()->TargetArmLength, (distFromAnchor + drawBackExtraDist), deltaTime);
	else
		UCameraProfile::TickUpdate(deltaTime);
}

void UControlCamProfile::Destroy()
{
	camManager->GetWorldTimerManager().ClearTimer(resetHandle);
}