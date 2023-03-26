// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PKClasses/PKProjectile.h"
#include "Main/NetPlayer.h"
#include "PainKillerFire.generated.h"

/**
 * 
 */
UCLASS()
class PK_API APainKillerFire : public APKProjectile
{
	GENERATED_BODY()
	
public:

	// Sets default values for this actor's properties
	APainKillerFire(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	/** called when projectile hits something */
	virtual void OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	virtual void OnBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool fromSweep, const FHitResult& Hit) override;
	
	virtual void Deactivate_Implementation() override;
protected:

	ANetPlayer* Owner;
	float DamageTimeout = 0.f;
	virtual void HitScan(float DeltaTime) override;
	TArray<FVector> Points;

	USoundBase* RotorHitEnemy;
	USoundBase* RotorHitWall;
	USoundCue* rotor_hit_enemy;
	USoundCue* rotor_hit_wall;
	
	USoundCue* PainRotorStartLoop;
	USoundBase* PainRotorStop;

	USoundBase* PainkillerShoot;
	FTimerDelegate PlaySoundTimerDelegate;
	UFUNCTION()
	void PlayRotorStopSound();
	int FramesToSkilp = 4;

	FTimerHandle TimerHandle;
};
