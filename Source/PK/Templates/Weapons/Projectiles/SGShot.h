// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PKClasses/PKProjectile.h"
#include "Main/NetPlayer.h"
#include "SGShot.generated.h"

/**
 * 
 */
UCLASS()
class PK_API ASGShot : public APKProjectile
{
	GENERATED_BODY()
	
public:

	// Sets default values for this actor's properties
	ASGShot(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;
	
protected:
	
	/*UPROPERTY()*/
	UParticleSystem* ShotgunHitWallParticle;
	
	FTimerDelegate ShotTimerDelegate;
	UFUNCTION()
	void SpreadShot();

	USoundBase* ShotgunShoot;
	USoundBase* HitSnd;
	USoundAttenuation* AttenuationSettings;
};
