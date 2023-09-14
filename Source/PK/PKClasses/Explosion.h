// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ParticleDefinitions.h"
#include "Components/BillboardComponent.h"
#include "Main/Util.h"
#include "Main/NetPlayer.h"
#include "GameFramework/DamageType.h"
#include "PKClasses/PKDamageType.h"
#include "Explosion.generated.h"

UCLASS()
class PK_API AExplosion : public AActor
{
	GENERATED_BODY()

	class USceneComponent* Scene;

	UPROPERTY(VisibleDefaultsOnly, Category = Properties)
	class UBillboardComponent* BillboardComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = Properties)
	class UTexture2D* SpriteTexture;

	float explosionStrength = 1500.f;
	
public:	
	// Sets default values for this actor's properties
	AExplosion(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
	float ExplosionRange = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Particles)
	UParticleSystem* ExplosionParticle;

protected:
	
	class USoundAttenuation* AttenuationSettings;
	USoundBase* RocketExplosionSnd;
	USoundBase* GrenadeComboExplosionSnd;

	// owning player
	ANetPlayerController* PC;
	AActor* DamageCauser;
	float Damage = /*15*/0.f;

	void DoExplosion();

	class TSubclassOf<UPKDamageType> DamageType;
	void ApplyDamage();
	void TraceApplyDmg(AActor* actor);
};
