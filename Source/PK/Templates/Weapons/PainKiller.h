// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PKClasses/PKWeapon.h"
#include "Main/Util.h"

#include "PainKiller.generated.h"

/**
 * 
 */
UCLASS()
class PK_API APainKiller : public APKWeapon
{
	GENERATED_BODY()
	

	APainKiller(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual bool Fire() override;

	virtual bool OnFinishFire() override;

	virtual bool AltFire() override;

	virtual bool OnFinishAltFire() override;
	
	virtual void OnHeadBack() override;

	virtual void PostInitializeComponents() override;

	virtual void OnChangeWeapon() override;

private:

protected:
	
	TArray<FName> BonesToHide;

	// Animations
	class UAnimMontage* startRot;
	class UAnimMontage* endRot;
	class UAnimMontage* startShot;
	class UAnimMontage* endShot;

	// Sounds
	USoundBase* PainkillerShoot;

	USoundCue* PainRotorStartLoop;
	USoundCue* PainRotorStop;
	USoundCue* PainHeadShot;
	USoundCue* PainHeadBack;
	
	FTimerHandle TimerHandle;
	bool bPainHeadShot;

	/*keep the user from repeatedly triggering 
	the AltFire while PainHead is on the way back*/
	bool bWithdraw;
	
	void Reload();
		
};
