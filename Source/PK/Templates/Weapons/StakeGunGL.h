// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PKClasses/PKWeapon.h"
#include "StakeGunGL.generated.h"

/**
 * 
 */
UCLASS()
class PK_API AStakeGunGL : public APKWeapon
{
	GENERATED_BODY()
	
	AStakeGunGL(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual bool Fire() override;

	virtual bool OnFinishFire() override;

	virtual bool AltFire() override;

	virtual bool OnFinishAltFire() override;

	virtual void HideStake(bool bHide) override;

	UFUNCTION()
	void Reload();

protected:

	// Animations
	UAnimMontage* fire;
	UAnimMontage* altfire;
	//Sounds
	USoundBase* stake_fire;
	USoundBase* grenade_fire;

	FTimerHandle TimerHandle;
};
