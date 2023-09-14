// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "Main/PKGameMode.h"
#include "Util.h"
#include "FFAGameMode.generated.h"

/**
 * 
 */
//UCLASS(minimalapi)
UCLASS()
class PK_API AFFAGameMode : public APKGameMode
{
	GENERATED_BODY()

	virtual void StartPlay() override;
	
public:
	AFFAGameMode(const FObjectInitializer& ObjectInitializer);

	/*virtual void PostLogin(APlayerController *Player) override;*/
	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void Logout(AController* Exiting) override;

	virtual void GenericPlayerInitialization(AController* C) override;

protected:

	virtual void HandleMatchHasStarted() override;
	
};
