// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraProfile.h"
#include "CamProfileManager.h"
#include "GameFramework/SpringArmComponent.h"

/**================================ GETTER/SETTER ================================*/

void UCameraProfile::Init(ACamProfileManager* camManager_)
{
	/* Guards */
	if (camManager_ == nullptr || camManager_->GetCameraBoom() == nullptr)
		return;

	/* set Ref and property */
	camManager = camManager_;
	camManager->GetCameraBoom()->ProbeSize = camProbeSize;
}

void UCameraProfile::TickUpdate(float deltaTime)
{
	/* Guards */
	if (camManager == nullptr || camManager->GetCameraBoom() == nullptr)
		return;

	/* update dist with player, lerp current with wanted  */
	camManager->GetCameraBoom()->TargetArmLength = FMath::Lerp(camManager->GetCameraBoom()->TargetArmLength,distFromAnchor,deltaTime);
}