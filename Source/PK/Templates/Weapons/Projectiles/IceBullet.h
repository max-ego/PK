// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PKClasses/PKProjectile.h"
#include "IceBullet.generated.h"

/**
 * 
 */
UCLASS()
class PK_API AIceBullet : public APKProjectile
{
	GENERATED_BODY()
	
public:

	// Sets default values for this actor's properties
	AIceBullet(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	/** called when projectile hits something */
	virtual void OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	virtual void OnBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool fromSweep, const FHitResult& Hit) override;

	virtual void Deactivate_Implementation() override;
	
protected:

	UParticleSystem* IceBulletHit;
	UParticleSystem* RocketTrail;
	UParticleSystemComponent* pscL;
	UParticleSystemComponent* pscR;

	USoundBase* ExplosionSnd;
	USoundBase* FreezerShoot;
};
