// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "HiddenCursorPlayerController.h"


AHiddenCursorPlayerController::AHiddenCursorPlayerController(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{
	bShowMouseCursor = false;
}

void AHiddenCursorPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetInputMode(FInputModeUIOnly());
}