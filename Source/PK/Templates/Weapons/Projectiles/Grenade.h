// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Templates/Weapons/Projectiles/Rocket.h"
#include "Grenade.generated.h"

/**
 * 
 */
UCLASS()
class PK_API AGrenade : public ARocket
{
	GENERATED_BODY()
	
	
public:

	// Sets default values for this actor's properties
	AGrenade(const FObjectInitializer& ObjectInitializer);
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	/** called when projectile hits something */
	virtual void OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	virtual void OnBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool fromSweep, const FHitResult& Hit) override;

	virtual void Deactivate_Implementation() override;

	UFUNCTION(NetMulticast, Reliable)
	void DeactivateQuiet();
	virtual void DeactivateQuiet_Implementation();

	UPROPERTY(ReplicatedUsing = OnRep_BounceSnd)
	uint32 bBounceSnd:1;
	UFUNCTION()
	virtual void OnRep_BounceSnd();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const override;

protected:

	float BounceSoundTimeOut = -1;
	bool IsSpawnedBehindWall();
};
