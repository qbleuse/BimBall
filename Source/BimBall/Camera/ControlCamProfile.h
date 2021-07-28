// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "CameraProfile.h"
#include "CamProfileManager.h"
#include "GameFramework/SpringArmComponent.h"
#include "ControlCamProfile.generated.h"

/**
 * CamProfile that listened to the input of the player 
 */
UCLASS()
class BIMBALL_API UControlCamProfile : public UCameraProfile
{
	GENERATED_BODY()
	
#pragma region UCONTROLCAMERAPROFILE
		/** UCONTROLCAMERAPROFILE */

	public:
		UControlCamProfile() {}
		~UControlCamProfile() {}

	/** END UCONTROLCAMERAPROFILE */
#pragma endregion

#pragma region CAMERA_LIMITATION
		/** CAMERA LIMITATION */

	protected:
		/* Upper limit of vertical angle of the camera */
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CameraLimits, meta = (AllowPrivateAccess = "true", ClampMin = "270.0", ClampMax = "360.0", UIMin = "270.0", UIMax = "360.0"))
		float maxPitchAngle = 310.0f;

		/* lower limit of vertical angle of the camera, the value should be between 270 and 360 */
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CameraLimits, meta = (AllowPrivateAccess = "true", ClampMin = "270.0", ClampMax = "360.0", UIMin = "270.0", UIMax = "360.0"))
		float minPitchAngle = 350.0f;

		/** END CAMERA LIMITATION */
#pragma endregion 

#pragma region CAMERA_SPEED
		/** CAMERA SPEED */
	
	protected:
		/* Acceleration of Camera's speed on yaw */
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CameraSpeed, meta = (AllowPrivateAccess = "true", UIMin = "1.0"))
		float camTurnAccel = 100.0f;

		/* Acceleration of Camera's speed on pitch */
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CameraSpeed, meta = (AllowPrivateAccess = "true", UIMin = "1.0"))
		float camLookUpAccel = 100.0f;

		/* Acceleration of Camera's speed on pitch */
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CameraSpeed, meta = (AllowPrivateAccess = "true", UIMin = "1.0"))
		float camLerpSpeed = 8.0f;

		/* current camera rotate speed, X = Yaw, Y = Pitch */
		UPROPERTY(BlueprintReadwrite, Category = CameraSpeed, meta = (AllowPrivateAccess = "true"))
		FVector2D camSpeed = FVector2D(0.0f, 0.0f);

		/** END CAMERA SPEED */
#pragma endregion 

#pragma region CAMERA_BEHAVIOR
		/** CAMERA BEHAVIOR */
	
	protected:
		FTimerHandle resetHandle;

		/** reset height delay */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
		float resetHeightDelay = 1.0f;

		/* Current joystick position */
		UPROPERTY(BlueprintReadOnly, Category = CameraBehavior, meta = (AllowPrivateAccess = "true"))
		FVector2D ctrlMvt = FVector2D(0.f, 0.f);

		/* Joystick before reset */
		UPROPERTY(BlueprintReadOnly, Category = CameraBehavior, meta = (AllowPrivateAccess = "true"))
		FVector2D resetCtrlMvt = FVector2D(0.f, 0.f);

		/** dist added when player run torwards Camera */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
		float drawBackExtraDist = 100.0f;

		/** Camera's time before camera get to the back of the player */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
		float camReturnDelay = 1.0f;

		/** Camera's time before camera get to the back of the player */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
		float camReturnEpsilon = 0.5f;

		UPROPERTY(BlueprintReadOnly, Category = CameraBehavior, meta = (AllowPrivateAccess = "true"))
		float ctrlYaw = 0.0f;

		/** Min Dist that left thumbstick should make before the direction 
		 * that the left thumbstick points is the same as the one you want 
		 * to go instead of the direction that you would go with ancient yaw of the cam before reset.*/
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CameraBehavior, meta = (AllowPrivateAccess = "true", UIMax = "0.0"))
		float resetEpsilon = 0.2f;

		/* boolean to know if cam reseting */
		UPROPERTY(BlueprintReadOnly, Category = CameraBehavior, meta = (AllowPrivateAccess = "true"))
		bool camResetYaw = false;

		/** dist before the init pitch of the camera will be changed to avoid collision with it */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CameraBehavior, meta = (AllowPrivateAccess = "true"))
		float heightDistLimit = 200.0f;

		/** for each heightUpUnit unit the camera is too close of the ground, pitch will add this value */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CameraBehavior, meta = (AllowPrivateAccess = "true"))
		float heightScale = 2.5f;

		/** Camera Dist From Ground */
		UPROPERTY(BlueprintReadOnly, Category = CameraBehavior, meta = (AllowPrivateAccess = "true"))
		float distFromGround = 0.0f;

	protected:
		/* reset the pitch of camera to camInitRot.Pitch */
		UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void ResetHeight();
		void ResetHeight_Implementation();

		/* method to prevent camera to collide with ground  */
		float LookForFloor(float pitch)const;

	public:
		/* get a value between 0 and 1 of how much the minPitch Angle should rise to maxPitchAngle */
		FORCEINLINE float GetExtraHeight(float pitch)const	{ return camManager ? (heightDistLimit - camManager->GetHeight(pitch))/heightDistLimit * heightScale : heightDistLimit; }

		/** END CAMERA BEHAVIOR */
#pragma endregion 

#pragma region CAM_INPUT
/** CAM_INPUT */

public:
	/* Input of Controller, move camera from joystick rate, changes pitch */
	virtual float LookUpAtRate	(float pitch, float Rate) override;
	/* Input of Controller, move camera from joystick rate, changes yaw */
	virtual float TurnAtRate	(float yaw, float Rate) override;
	/* Input of Controller, move camera from mouse rate, changes pitch */
	virtual float LookUp		(float pitch, float Rate) override;
	/* Input of Controller, move camera from mouse rate, changes yaw */
	virtual float Turn			(float yaw, float Rate) override;

	/* Input of Controller, get the forwardVec of cam to move character forward */
	virtual FVector GetFrwdVec	(float yaw, float Value) override;
	/* Input of Controller, get the rightVec of cam to move character right */
	virtual FVector GetRightVec	(float yaw, float Value) override;

	/* set the yaw and pitch of cam to value given in parameter, can disableInput, during movement */
	virtual void	Reset(float yaw, float pitch)override;

	/** END CAM_INPUT */
#pragma endregion

#pragma region GETTER/SETTER
/** GETTER/SETTER */

public:
	/* to call when created */
	virtual void Init(class ACamProfileManager* camManager_)override;
	/* to call each tick, update some variable */
	virtual void TickUpdate(float deltaTime)override;
	/* to call when destroy */
	virtual void Destroy()override;

	/** END GETTER/SETTER */
#pragma endregion

};
