// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Camera/CameraActor.h"
#include "WarmUpView.generated.h"

/**
 * 
 */
UCLASS()
class PK_API AWarmUpView : public ACameraActor
{
	GENERATED_BODY()

public:
	AWarmUpView(const FObjectInitializer& ObjectInitializer);
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;
	

protected:

	FRotator Rot = FRotator();
	
	UFUNCTION()
	void SetRot();
	
	bool bCanTick = false;
	FTimerDelegate SetRotTimerDelegate;
};
