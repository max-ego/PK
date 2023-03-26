// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "Main/Util.h"
#include "MainMenuPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PK_API AMainMenuPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AMainMenuPlayerController(const FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaSeconds) override;

	virtual void BeginPlay() override;

protected:
	void OnEscape();

	UFUNCTION()
	void CenterCursor();

	FTimerDelegate CenterCursorTimerDelegate;
	FTimerHandle TimerHandle;
};
