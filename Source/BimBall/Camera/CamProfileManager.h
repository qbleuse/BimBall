// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BimBall/Entity/Player/BimBallCharacter.h"
#include "CamProfileManager.generated.h"

UCLASS(Blueprintable, BlueprintType)
class BIMBALL_API ACamProfileManager : public AActor
{
	GENERATED_BODY()
	
#pragma region ACAMPROFILEMANAGER
	/** ACAMPROFILEMANAGER */
	
	protected:
		// Called when the game starts or when spawned
		virtual void BeginPlay() override;
	
		// Called every frame
		virtual void Tick(float DeltaTime) override;

	public:
		// Sets default values for this actor's properties
		ACamProfileManager();

	/** END ACAMPROFILEMANAGER */
#pragma endregion

#pragma region CAMERA_PROFILE
	/** CAMERA_PROFILE */

	protected:
		/* Current used Cam Profile */
		UPROPERTY(Transient, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraProfile* camProfile = nullptr;	

		/* Max Dist that Ball Cam can have before beinbg disabled */
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
		float maxFocusDist = 0.0f;

	public:
		/* basic CamProfile */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<class UCameraProfile> basicCamProfile;
	
		/* fight CamProfile */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<class UCameraProfile> fightCamProfile;
	
		/* ball CamProfile */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<class UCameraProfile> ballCamProfile;

		/* boolean to know if fight cam profile is activated */
		UPROPERTY(BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
		bool isFightCamActivated = false;
	
		/* boolean to know if ball cam profile is activated */
		UPROPERTY(BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
		bool isBallCamActivated = false;

	public:
		/* function to setup one of the cam profile above */
		UFUNCTION(BlueprintCallable)
		void SetCamProfile(TSubclassOf<class UCameraProfile> aCamProfile);

	/** END CAMERA_PROFILE */
#pragma endregion 

#pragma region CAMERA_COMPONENT
	/** CAMERA_COMPONENT */

	protected:
		/** Camera boom positioning the camera behind the character */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom = nullptr;
	
		/** Follow camera */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera = nullptr;

	public:
		/** Returns CameraBoom subobject **/
		FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

		/** Returns FollowCamera subobject **/
		FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

		float GetHeight(float pitch)const;


	/** END CAMERA_COMPONENT */
#pragma endregion 

#pragma region CAMERA_INPUT
	/** CAMERA_INPUT */
	protected:
		/* sensibility of the Cam Thumbstick to disable ball Cam*/
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CameraSpeed, meta = (AllowPrivateAccess = "true", UIMin = "0.0"))
		float camInputSensibility = 1.0f;

	public:
		/* input to setup and remove ball cam */
		UFUNCTION(BlueprintCallable)
		void BallCamActivate();

		/**
		 * Called via input to turn look up/down at a given rate.
		 */
		UFUNCTION(BlueprintNativeEvent)
		void LookUpAtRate(float Rate);
		void LookUpAtRate_Implementation(float Rate);
	
		/**
		 * Called via input to turn at a given rate.
		 */
		UFUNCTION(BlueprintNativeEvent)
		void TurnAtRate(float Rate);
		void TurnAtRate_Implementation(float Rate);

		/**
		* Called via input to turn look up/down with mouse.
		*/
		UFUNCTION(BlueprintNativeEvent)
		void LookUp(float Rate);
		void LookUp_Implementation(float Rate);

		/**
		* Called via input to turn with mouse.
		*/
		UFUNCTION(BlueprintNativeEvent)
		void Turn(float Rate);
		void Turn_Implementation(float Rate);

		/* Set yaw given in parameter to the Camera Boom */
		void SetYaw(float yaw)const;

		/* Set yaw given in parameter to the Camera Boom */
		void SetPitch(float pitch)const;

		/* Get ForwardVec of Cam, to move character */
		FVector GetFrwdVec(float Value);

		/* Get ForwardVec of Cam, to move character */
		FVector GetRightVec(float Value);

		/* Input to set given yaw, and pitch to Camera Boom */
		UFUNCTION(BlueprintCallable)
		void ResetCam(float wantedYaw, float wantedPitch);

	/** END CAMERA_INPUT */
#pragma endregion 

#pragma region ENTITIES_REF
	/** ENTITIES_REF */

	protected:
		/* Character That show th camera */
		class ABimBallCharacter*	playerChar		= nullptr;

		/* enemyManager to get enemies position */
		class UEnemyManager*		enemyManager	= nullptr;
	
	protected:
		/* method to know if player is considered engaged */
		FORCEINLINE bool IsPlayerEngaged()const { return (playerChar ? playerChar->IsPlayerEngaged() : false); }

	public:
		/* method to set the ref of the character we show */
		void SetPlayer(class ABimBallCharacter* playerChar_);

	/** END ENTITIES_REF */
#pragma endregion 

#pragma region CAMERA_EFFECT
/** CAMERA_EFFECT */

	protected:
		/* handle to stop shake after duration */
		FTimerHandle shakeHandle;

	public:
		/* Play a Camera Shake for duration (if Duration > 0.0, plays indefinitly)*/
		UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void Shake					(TSubclassOf<class UCameraShake> shakeClass, float duration = -1.0f);
		void Shake_Implementation	(TSubclassOf<class UCameraShake> shakeClass, float duration = -1.0f);

		/* stop wanted Camera Shake */
		UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void StopShake					(class UCameraShake* cameraShake);
		void StopShake_Implementation	(class UCameraShake* cameraShake);

		/* stop all Camera Shake */
		UFUNCTION(BlueprintCallable)
		void StopAllShake();

		/** END CAMERA_EFFECT */
#pragma endregion 
};
