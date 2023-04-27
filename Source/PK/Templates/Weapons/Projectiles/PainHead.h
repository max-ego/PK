// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PKClasses/PKProjectile.h"
#include "Main/NetPlayer.h"
#include "PainHead.generated.h"

/**
 * 
 */
UCLASS()
class PK_API APainHead : public APKProjectile
{
	GENERATED_BODY()

	
public:

	// Sets default values for this actor's properties
	APainHead(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	void PostInitializeComponents() override;

	/** called when projectile hits something */
	virtual void OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	virtual void OnBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool fromSweep, const FHitResult& Hit) override;
	
	virtual void Deactivate_Implementation() override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const override;

protected:

	UPROPERTY(ReplicatedUsing = OnRep_BeamToggle)
	bool bHideBeam = false;
	UFUNCTION()
	virtual void OnRep_BeamToggle();
	virtual void HideBeam(bool Hide);
	UParticleSystem* HitEnemyParticle;
	
	float DamageTimeout = 0.f;

	UPROPERTY(ReplicatedUsing = OnRep_BeamToggleSnd)
	bool bShock = false;
	UFUNCTION()
	virtual void OnRep_BeamToggleSnd();
	virtual void BeamShockSnd(bool bShockSnd);

	virtual void OnRep_PainHeadWithdraw() override;

	virtual void HookUpEnemy(AActor* OtherActor) override;

	virtual void BeamIntersection(float DeltaTime);

	FORCEINLINE void DestroyBeam()
	{
		if (HeadBeamComp != nullptr
			&& HeadBeamComp->IsValidLowLevel()
			&& !HeadBeamComp->IsPendingKill())
		{
			HeadBeamComp->DestroyComponent();
		}

		ElectroLoopAudioComp->Stop();
	}

	ANetPlayer* Owner;
	UParticleSystemComponent* HeadBeamComp;

	// sounds
	USoundBase* electro_loop;
	USoundBase* electro_shock_loop;

	USoundCue* ElectroShockLoop;
	USoundCue* ElectroLoop;
	UAudioComponent*  ElectroLoopAudioComp;

	USoundBase* PainkillerShoot;
	USoundCue* PainHeadBack;
	USoundBase* PainkillerStick;
};
