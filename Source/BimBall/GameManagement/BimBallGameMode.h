// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BimBallGameMode.generated.h"

UCLASS(minimalapi)
class ABimBallGameMode : public AGameModeBase
{
	GENERATED_BODY()

#pragma region ABIMBALLGAMEMODE
	/** ABIMBALLGAMEMODE */

protected:
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)override;
	virtual void BeginPlay()override;

public:
	ABimBallGameMode();

	/** END ABIMBALLGAMEMODE */
#pragma endregion

#pragma region LOSE_WIN
	/** LOSE_WIN */

protected:
	UPROPERTY(BlueprintReadOnly, Category = WinLoseCondition, meta = (AllowPrivateAccess = "true"))
	bool bWin = false;

	UPROPERTY(BlueprintReadOnly, Category = WinLoseCondition, meta = (AllowPrivateAccess = "true"))
	bool bLose = false;

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Win();
	void Win_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Lose();
	void Lose_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ResetWinLose();
	void ResetWinLose_Implementation();

	/** END LOSE_WIN */
#pragma endregion

#pragma region ENEMY_MANAGER
	/** ENEMY_MANAGER */

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Enemy, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UEnemyManager> subEnemyManager;

	UPROPERTY(BlueprintReadOnly, Category = Enemy, meta = (AllowPrivateAccess = "true"))
	class UEnemyManager* enemyManager = nullptr;



public:

	FORCEINLINE class UEnemyManager*		GetEnemyManager()const	{ return enemyManager; }

	void SetPlayerChar(class ABimBallCharacter* playerChar_);



	/** END ENEMY_MANAGER */
#pragma endregion

#pragma region YARD
	/** YARD */
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Target, meta = (AllowPrivateAccess = "true"))
	TArray<class AYard* > yards;

	/** END YARD */
#pragma endregion

};



