// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PKClasses/PKProjectile.h"
#include "Main/NetPlayer.h"
#include "MiniGunFire.generated.h"

/**
 * 
 */
UCLASS()
class PK_API AMiniGunFire : public APKProjectile
{
	GENERATED_BODY()
	
public:

	// Sets default values for this actor's properties
	AMiniGunFire(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	virtual void Deactivate_Implementation() override;

protected:

	UFUNCTION(Client, Reliable)
	void StopFire();
	void StopFire_Implementation();
	
	float BulletTimeout = 0.f;
	UParticleSystem* MinigunHitWallParticle;
	UParticleSystem* ShotgunHitWallParticle;
	
	ANetPlayer* Owner;
	void HitScan(float DeltaTime) override;
	float AltFireDamage = 100.f;

	USoundCue* Bullets;
	USoundBase* MGunStop;

	uint32 ParticlesPerHitSound = 0;
};
