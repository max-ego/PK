// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Templates/Weapons/Projectiles/Stake.h"
#include "StickedStake.generated.h"

/**
 * 
 */
UCLASS()
class PK_API AStickedStake : public AStake
{
	GENERATED_BODY()
	
public:

	// Sets default values for this actor's properties
	AStickedStake(const FObjectInitializer& ObjectInitializer);

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** called when projectile hits something */
	virtual void OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	virtual void OnBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool fromSweep, const FHitResult& Hit) override;

	virtual void Deactivate_Implementation() override;
	
	
};
