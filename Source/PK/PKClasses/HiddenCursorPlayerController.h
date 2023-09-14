// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "HiddenCursorPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PK_API AHiddenCursorPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AHiddenCursorPlayerController(const FObjectInitializer& ObjectInitializer);
	
	virtual void BeginPlay() override;
};
