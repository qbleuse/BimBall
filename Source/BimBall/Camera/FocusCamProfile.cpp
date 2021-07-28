// Fill out your copyright notice in the Description page of Project Settings.


#include "FocusCamProfile.h"
#include "CamProfileManager.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

/**================================ CAM_INPUT ================================*/

float UFocusCamProfile::LookUpAtRate(float pitch, float Rate)
{
	return camInitRot.Pitch;
}

float UFocusCamProfile::TurnAtRate(float yaw, float Rate)
{
	if (target == nullptr)
		return yaw;
	
	/* the yaw is changed to always look the target */

	FVector dir = target->GetActorLocation() - camManager->GetActorLocation();
	return dir.Rotation().Yaw;
}

float UFocusCamProfile::LookUp(float pitch, float Rate)
{
	return camInitRot.Pitch + Rate;
}

float UFocusCamProfile::Turn(float yaw, float Rate)
{
	return yaw + Rate;
}

FVector UFocusCamProfile::GetFrwdVec(float yaw, float Value)
{
	return FRotationMatrix(FRotator(0.0, yaw, 0.0f)).GetUnitAxis(EAxis::X);
}

FVector UFocusCamProfile::GetRightVec(float yaw, float Value)
{
	return FRotationMatrix(FRotator(0.0, yaw, 0.0f)).GetUnitAxis(EAxis::Y);
}

/**================================ GETTER/SETTER ================================*/

void UFocusCamProfile::Init(ACamProfileManager* camManager_)
{
	Super::Init(camManager_);

	/* Guards */
	if (camManager == nullptr || camManager->GetCameraBoom() == nullptr)
		return;
	
	camManager->GetCameraBoom()->bEnableCameraRotationLag	= true;
	camManager->GetCameraBoom()->bEnableCameraLag			= true;
}

void UFocusCamProfile::TickUpdate(float deltaTime)
{
	/* Guards */
	if (!target || !camManager || !camManager->GetCameraBoom() || !camManager->GetFollowCamera())
		return;

	camManager->GetFollowCamera()->SetRelativeRotation(FRotator(((target->GetActorLocation() - camManager->GetActorLocation()) * offSetScale).Rotation().Pitch,0.0f,0.0f));

	camManager->GetCameraBoom()->CameraRotationLagSpeed = FMath::Lerp(camManager->GetCameraBoom()->CameraRotationLagSpeed, camLerpSpeed,deltaTime);
	camManager->GetCameraBoom()->CameraLagSpeed			= FMath::Lerp(camManager->GetCameraBoom()->CameraLagSpeed, camMaxSpeed, deltaTime);
	
	/* The base is from the law of sinus in triangles :
	 *																/-> cam
	 *													  beta <-( /
	 *														/	  /
	 *									    			 /		 / 
	 *												/			/
	 *											/			   /	
	 *										/				  /	
	 *									/    				 /	
	 *								/	    				/		
	 *						 c ->/						   /		
	 *						/			    			  /		
	 *					/								 /<- dirSize/b		
	 *				/									/			
	 *			/									   /			
	 *		/)->alpha	(camInitRot.Pitch - 180.0f)->(/
	 *	  --------------------------------------------/
	 *		^					^					 ^
	 *		|					|					 |
	 *		target				a					 player
	 * 
	 *  a/sin(beta) = b/sin(alpha) = c/(camInitRot.Pitch - 180.0f)
	 * 
	 * c = a/sin(camFocusFov) * sin(camInitRot.Pitch - 180.0f)  
	 * 
	 * tecnically, we compute c and make it for b, cause it has a better game feel
	 * 
	 */
	float dirSize = (target->GetActorLocation() - camManager->GetActorLocation()).Size() / FMath::Sin(FMath::DegreesToRadians(camFocusFov))
					* FMath::Sin(FMath::DegreesToRadians(camInitRot.Pitch - 180.0f));

	camManager->GetCameraBoom()->TargetArmLength = FMath::Clamp(FMath::Lerp(camManager->GetCameraBoom()->TargetArmLength, dirSize, deltaTime),distFromAnchor,maxDrawBack);
}

void UFocusCamProfile::Destroy()
{
}