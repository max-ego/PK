// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "PKClasses/PKProjectile.h"
#include "TmpProjectile.generated.h"

UCLASS()
class PK_API ATmpProjectile : public APKProjectile
{
	GENERATED_BODY()	

public:

	// Sets default values for this actor's properties
	ATmpProjectile(const FObjectInitializer& ObjectInitializer);

	/** called when projectile hits something */
	virtual void OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	virtual void OnBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool fromSweep, const FHitResult& Hit) override;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

protected:

	/** ball bounce sound */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class USoundBase* HitSound;

	class USoundAttenuation* AttenuationSettings;
};
