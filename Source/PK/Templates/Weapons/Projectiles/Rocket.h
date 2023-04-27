// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Main/NetPlayer.h"
#include "PKClasses/PKProjectile.h"
#include "Rocket.generated.h"

UENUM()
enum ERocketType{
	rocket = 0x00,
	grenade = 0x01,
};

UCLASS()
class PK_API ARocket : public APKProjectile
{
	GENERATED_BODY()
	
public:

	// Sets default values for this actor's properties
	ARocket(const FObjectInitializer& ObjectInitializer);
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	/** called when projectile hits something */
	virtual void OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	virtual void OnBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool fromSweep, const FHitResult& Hit) override;
	
	virtual void Deactivate_Implementation() override;

	static float ExplosionRange;
	static FVector GetExplosionVelocity(FVector ExplosionLoc, UMovementComponent* ActorMovementComp);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const override;

protected:

	UPROPERTY(ReplicatedUsing = OnRep_ExplosionLoc)
	TArray<FVector_NetQuantize10> ExplosionLocRot = {};
	UFUNCTION()
	void OnRep_ExplosionLoc();

	uint8 RocketType = ERocketType::rocket;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class USoundBase* RocketFlySound;
	
	USoundBase* ExplosionSnd;
	USoundBase* rl_shoot;
	USoundCue* RocketFlyingNoiseLoop;
	USoundBase* grenade_fire;
	USoundBase* grenade_bounce;

	class USoundAttenuation* AttenuationSettings;

	UParticleSystem* RocketTrail;
		
	virtual void ApplyDamage(AActor* OtherActor = 0, TSubclassOf<UPKDamageType> DamageTypeClass = 0) override;
};
