// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BimBall/Camera/CameraProfile.h"
#include "FocusCamProfile.generated.h"

/**
 * CamProfile that changes cam position to see the character and the target
 */
UCLASS()
class BIMBALL_API UFocusCamProfile : public UCameraProfile
{
	GENERATED_BODY()

#pragma region UFOCUSCAMERAPROFILE
	/** UFOCUSCAMERAPROFILE */

	public:
		UFocusCamProfile() {}
		~UFocusCamProfile() {}

	/** END UFOCUSCAMERAPROFILE */
#pragma endregion

#pragma region FOCUS
/** FOCUS */

	protected:
		/* angle saying how much the player and the ball takes in the fov */
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CameraSpeed, meta = (AllowPrivateAccess = "true", UIMin = "0.0", UIMax = "90.0"))
		float camFocusFov = 60.0f;

		/* how much the look of the camera is offset of the player to the ball */
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CameraSpeed, meta = (AllowPrivateAccess = "true", UIMin = "0.0", UIMax = "1.0"))
		float offSetScale = 0.25f;

		/* how much the arm of the cam ca&n be drawn back */
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CameraSpeed, meta = (AllowPrivateAccess = "true", UIMin = "0.0"))
		float maxDrawBack = 2000;

	public:
		/* target to focus */
		UPROPERTY(BlueprintReadOnly, Category = CameraFocus, meta = (AllowPrivateAccess = "true"))
		AActor* target = nullptr;

	public:

		UFUNCTION(BlueprintCallable)
		FORCEINLINE float GetMaxDrawBack() { return maxDrawBack; }

		UFUNCTION(BlueprintCallable)
		FORCEINLINE float GetMaxDistFromTarget() { return ((1.0f / offSetScale) * maxDrawBack 
													/ FMath::Sin(FMath::DegreesToRadians(camFocusFov))
													* FMath::Sin(FMath::DegreesToRadians(camInitRot.Pitch - 180.0f))); }

	/** END FOCUS */
#pragma endregion

#pragma region CAMERA_SPEED
		/** CAMERA SPEED */

	protected:
		/* Camera's speed */
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CameraSpeed, meta = (AllowPrivateAccess = "true", UIMin = "1.0"))
		float camLerpSpeed = 8.0f;

	/** END CAMERA SPEED */
#pragma endregion

#pragma region CAM_INPUT
	/** CAM_INPUT */

	public:
		/* These are disabled Basically */
		virtual float	LookUpAtRate(float pitch, float Rate) override;
		virtual float	TurnAtRate	(float yaw, float Rate) override;
		virtual float	LookUp		(float pitch, float Rate) override;
		virtual float	Turn		(float yaw, float Rate) override;

		/* these are not */

		/* Input of Controller, get the forwardVec of cam to move character forward */
		virtual FVector GetFrwdVec	(float yaw, float Value) override;
		/* Input of Controller, get the rightVec of cam to move character right */
		virtual FVector GetRightVec	(float yaw, float Value) override;

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
