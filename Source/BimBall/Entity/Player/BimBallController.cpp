// Fill out your copyright notice in the Description page of Project Settings.

#include "BimBallController.h"
#include "BimBallCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "BimBall/Camera/CamProfileManager.h"
#include "Engine/World.h"
#include "TimerManager.h"

/**================================ ABIMBALLCONTROLLER ================================*/

void ABimBallController::BeginPlay()
{
	playerChar = Cast<ABimBallCharacter>(GetPawn());
	playerChar->SetupCamManager(InputComponent);
	
}

void ABimBallController::Disable()
{
	DisableInput(this);
}

void ABimBallController::Enable()
{
	EnableInput(this);
}

void ABimBallController::SetupInputComponent()
{
	Super::SetupInputComponent();

	/* Set up gameplay key bindings */
	check(InputComponent);

	/* Jump Input */
	InputComponent->BindAction("Jump", IE_Pressed,	this, &ABimBallController::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ABimBallController::StopJumping);

	/* Sprint Input */
	InputComponent->BindAction("Charge", IE_Pressed,	this, &ABimBallController::Charge);
	InputComponent->BindAction("Charge", IE_Released,	this, &ABimBallController::StopCharging);

	/* Development Input */
	//InputComponent->BindAction("Restart", IE_Pressed,	this, &ABimBallController::ReloadLevel);
	//InputComponent->BindAction("Quit", IE_Pressed,		this, &ABimBallController::QuitGame);

	/* Atk Input */
	InputComponent->BindAction("Throw", IE_Pressed,	this, &ABimBallController::AimOrCatchBall);
	InputComponent->BindAction("Throw", IE_Released, this, &ABimBallController::Throw);
	InputComponent->BindAction("Spin", IE_Pressed, this, &ABimBallController::Spin);
	InputComponent->BindAction("BasicAttack", IE_Pressed, this, &ABimBallController::BasicAttack);
	InputComponent->BindAction("GrabAndFling", IE_Pressed, this, &ABimBallController::GrabEntityAuto);
	InputComponent->BindAction("GrabAndFling", IE_Released, this, &ABimBallController::FlingEntity);
	InputComponent->BindAction("Drop", IE_Pressed, this, &ABimBallController::DropBall);

	/* Movement Input */
	InputComponent->BindAxis("MoveForward", this, &ABimBallController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ABimBallController::MoveRight);

	InputComponent->BindAction("BallCam", IE_Pressed, this, &ABimBallController::ResetCamProfile);
}

/**================================ SPECIAL_BEHAVIOR ================================*/

void ABimBallController::IncapacitateFor_Implementation(float duration)
{
	incapacitate = true;
	if (duration > 0.0f)
	{
		GetWorldTimerManager().SetTimer(incapitateHandle, this, &ABimBallController::EndIncapacitate, duration);
	}
}

void ABimBallController::EndIncapacitate_Implementation()
{
	incapacitate = false;
}

/**================================ DEV_INPUT ================================*/

void ABimBallController::ReloadLevel()
{
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}

void ABimBallController::QuitGame()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), this, EQuitPreference::Quit, false);
}

/**================================ ATK_INPUT ================================*/

void ABimBallController::AimOrCatchBall()
{
	if (playerChar && !incapacitate)
	{
		if (playerChar->HasBall())
		{
			playerChar->Aim();
		}
		else
		{
			playerChar->CatchNearBall();
		}
	}
}

void ABimBallController::CatchNearBall()
{
	if (playerChar && !incapacitate)
	{
		playerChar->CatchNearBall();
	}
}

void ABimBallController::DropBall()
{
	if (playerChar && !incapacitate)
	{
		playerChar->DropBall();
	}
}

void ABimBallController::Aim()
{
	if (playerChar && !incapacitate)
	{
		playerChar->Aim();
	}
}

void ABimBallController::Throw()
{
	if (playerChar && !incapacitate && playerChar->GetIsAiming())
	{
		playerChar->Throw();
	}
}

void ABimBallController::Spin()
{
	if (playerChar && !incapacitate)
	{
		playerChar->Spin();
	}
}

void ABimBallController::BasicAttack()
{
	if (playerChar && !incapacitate)
	{
		playerChar->BasicAttack();
	}
}

void ABimBallController::GrabEntityAuto()
{
	if (playerChar && !incapacitate)
	{
		playerChar->GrabEntityAuto();
	}
}

void ABimBallController::FlingEntity()
{
	if (playerChar && !incapacitate)
	{
		playerChar->FlingEntity();
	}
}

/**================================ MVT_INPUT ================================*/

void ABimBallController::Jump()
{
	if (playerChar && !incapacitate)
	{
		playerChar->SetIsAiming(false);
		playerChar->Jump();
	}
}


void ABimBallController::StopJumping()
{
	if (playerChar && !incapacitate)
	{
		playerChar->StopJumping();
	}
}

void ABimBallController::MoveForward(float value)
{
	if (playerChar && !incapacitate)
	{
		playerChar->MoveForward(value);
	}
}

void ABimBallController::MoveRight(float value)
{
	if (playerChar && !incapacitate)
	{
		playerChar->MoveRight(value);
	}
}

void ABimBallController::Charge()
{
	if (playerChar && !incapacitate)
	{
		playerChar->Charge();
	}
}

void ABimBallController::StopCharging()
{
	if (playerChar && !incapacitate)
	{
		playerChar->StopCharging();
	}
}

/**================================ CAM_INPUT ================================*/

void ABimBallController::ResetCamProfile()
{
	if (playerChar && !incapacitate)
	{
		playerChar->ResetCamProfile();
	}
}