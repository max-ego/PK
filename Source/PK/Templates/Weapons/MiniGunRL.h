// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PKClasses/PKWeapon.h"
#include "MiniGunRL.generated.h"

/**
 * 
 */
UCLASS()
class PK_API AMiniGunRL : public APKWeapon
{
	GENERATED_BODY()
	
	AMiniGunRL(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual bool Fire() override;

	virtual bool OnFinishFire() override;

	virtual bool AltFire() override;

	virtual bool OnFinishAltFire() override;

private:

	// Animations
	UAnimMontage* fire;
	UAnimMontage* chainshot;
	UAnimMontage* endcshot;

	//Sounds
	USoundBase* rl_shoot;

	USoundCue* Bullets;
	USoundCue* NoBullets;
	USoundCue* MGunStop;

	bool bAltFireOn = false;
};
