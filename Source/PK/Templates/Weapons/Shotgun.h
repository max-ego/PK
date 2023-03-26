// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PKClasses/PKWeapon.h"
#include "Shotgun.generated.h"

/**
 * 
 */
UCLASS()
class PK_API AShotgun : public APKWeapon
{
	GENERATED_BODY()
	
	AShotgun(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual bool Fire() override;

	virtual bool OnFinishFire() override;

	virtual bool AltFire() override;

	virtual bool OnFinishAltFire() override;

protected:
	
private:

	// Animations
	UAnimMontage* fire;
	UAnimMontage* altfire;

	// Sounds
	UAudioComponent* SGAudioComponent;

	USoundCue* Shotgun;
	USoundCue* Freezer;
	
};
