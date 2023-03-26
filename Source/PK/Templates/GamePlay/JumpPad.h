// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Main/Util.h"
#include "Main/PKCache.h"
#include "GameFramework/Character.h"
#include "JumpPad.generated.h"

UCLASS()
class PK_API AJumpPad : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AJumpPad(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	/** Box collision component */
	UPROPERTY(VisibleDefaultsOnly, Category = JumpPad)
	class UBoxComponent* CollisionComp;

	UPROPERTY(VisibleDefaultsOnly, Category = Actor)
	class UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
	float JumpStrength = 2000;
	
	class USoundAttenuation* AttenuationSettings;
	
	UFUNCTION()
	void OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	UFUNCTION()
	void OnBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool fromSweep, const FHitResult& Hit);

protected:
	TScriptDelegate<FWeakObjectPtr> HitDelegate;
	TScriptDelegate<FWeakObjectPtr> BeginOverlapDelegate;

	USoundBase* snd;

	// avoid repeated sound play due to movement correction on the client
	float SndDelay = 0.f;
};
