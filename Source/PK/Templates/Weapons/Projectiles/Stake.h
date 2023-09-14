// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PKClasses/PKProjectile.h"
#include "Main/NetPlayer.h"
//#include "GrenadeCombo.h"
#include "Stake.generated.h"

/**
 * 
 */
UCLASS()
class PK_API AStake : public APKProjectile
{
	GENERATED_BODY()
	
	class UStaticMeshComponent* StaticMeshComponentGrenade;

public:

	// Sets default values for this actor's properties
	AStake(const FObjectInitializer& ObjectInitializer);
	
	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** called when projectile hits something */
	virtual void OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	virtual void OnBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool fromSweep, const FHitResult& Hit) override;

	virtual void Deactivate_Implementation() override;

protected:

	float CalcTimeToLive();
		
	FTimerHandle BurnStakeTimerHandle;
	void BurnStake();

	UFUNCTION(NetMulticast, Reliable)
	void ComboFireFX();
	void ComboFireFX_Implementation();
	APKProjectile* GrenadeCombo;
	
	bool bMissileCollision = false;
	UFUNCTION(NetMulticast, Reliable)
	void OnMissileCollision();
	FORCEINLINE void OnMissileCollision_Implementation()
	{
		bMissileCollision = true;
		Deactivate();
	};

	class USoundAttenuation* AttenuationSettings;
	USoundBase* StakeHitSnd;
	USoundBase* StakeHitComboSnd;
	USoundBase* StakeFireComboSnd;
	USoundBase* StakeFire;

	USoundCue* StakeShield;
	USoundCue* StakeDefault;
	USoundCue* StakeOnflyLoop;

	UParticleSystem* RocketTrail;
	UParticleSystemComponent* Trail;

	ANetPlayer* Owner;
	FVector Start;
	virtual void HitScan() override;
};
