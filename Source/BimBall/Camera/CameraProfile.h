// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CameraProfile.generated.h"

/**
 * Base of CameraProfile Class, a clas  to control the position of the camera,
 * depending on the input of the player.
 */
UCLASS(Blueprintable, BlueprintType)
class BIMBALL_API UCameraProfile : public UObject
{
	GENERATED_BODY()

#pragma region UCAMERAPROFILE
	/** UCAMERAPROFILE */

	public:
		UCameraProfile() {}
		~UCameraProfile(){}

	/** END UCAMERAPROFILE */
#pragma endregion

#pragma region MANAGER_REF
/** MANAGER_REF */

	protected:
		/* ptr to the manager class, useful to get CameraBoom and Camera*/
		class ACamProfileManager* camManager = nullptr;

	/** END MANAGER_REF */
#pragma endregion

#pragma region CAM_INFO
	/** CAM_INFO */

	protected:
		/* distance of the camera from the player */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
		float distFromAnchor = 300.0f;

		/* maximum speed of Camera*/
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CameraSpeed, meta = (AllowPrivateAccess = "true", UIMin = "0.0", UIMax = "360.0"))
		float camMaxSpeed = 90.0f;

		/* size of probe that collide with terrain */
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CameraSpeed, meta = (AllowPrivateAccess = "true", UIMin = "0.0"))
		float camProbeSize = 12.0f;

	public:

		/* Cam Initial Position that player will come back to when no input is done (Pitch,Yaw,Roll) */
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		FRotator camInitRot = FRotator(330.0f, 0.0f, 0.0f);

		/* max dist from the camera for the player to being hidden */
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CameraSpeed, meta = (AllowPrivateAccess = "true", UIMin = "0.0"))
		float playerDisapearanceDist = 200.0f;

	/** END CAM_INFO */
#pragma endregion

#pragma region CAM_INPUT
	/** CAM_INPUT */

	public:
		/* Input of Controller, move camera from joystick rate, changes pitch */
		virtual float LookUpAtRate(float pitch, float Rate) { return pitch; }
		/* Input of Controller, move camera from joystick rate, changes yaw */
		virtual float TurnAtRate(float yaw, float Rate)		{ return yaw; }
		/* Input of Controller, move camera from mouse rate, changes pitch */
		virtual float LookUp(float pitch, float Rate)		{ return pitch; }
		/* Input of Controller, move camera from mouse rate, changes yaw */
		virtual float Turn(float yaw, float Rate)			{ return yaw; }
	
		/* Input of Controller, get the forwardVec of cam to move character forward */
		virtual FVector GetFrwdVec(float yaw, float Value) { return FVector(0.0f); }
		/* Input of Controller, get the rightVec of cam to move character right */
		virtual FVector GetRightVec(float yaw, float Value) { return FVector(0.0f); }

		/* set the yaw and pitch of cam to value given in parameter, can disableInput, during movement */
		virtual void	Reset(float yaw, float pitch) {}

	/** END CAM_INPUT */
#pragma endregion

#pragma region GETTER/SETTER
/** GETTER/SETTER */

	public:
		/* to call when created */
		virtual void Init(class ACamProfileManager* camManager_);
		/* to call each tick, update some variable */
		virtual void TickUpdate(float deltaTime);
		/* to call when destroy */
		virtual void Destroy() {}

	/** END GETTER/SETTER */
#pragma endregion

};
