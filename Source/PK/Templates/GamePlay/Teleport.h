// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Main/Util.h"
#include "Main/NetPlayer.h"
#include "Main/PKCharacterMovementComponent.h"
#include "TeleFrag.h"
#include "Teleport.generated.h"

UCLASS()
class PK_API ATeleport : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATeleport(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	/** Box collision component */
	UPROPERTY(VisibleDefaultsOnly, Category = Teleport)
	class UBoxComponent* CollisionComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
	class ATargetPoint* DestinationPiont/* = (ATargetPoint*)0*/;
	
	UFUNCTION()
	void OnBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool fromSweep, const FHitResult& Hit);
	
protected:
	TScriptDelegate<FWeakObjectPtr> BeginOverlapDelegate;

	void PlayTeleportSnd();

	UFUNCTION(NetMulticast, UnReliable)
	void PlaySnd(int32 PlayerId);
	void PlaySnd_Implementation(int32 PlayerId);	

	USoundBase* snd;

	FVector DestLocation = FVector::ZeroVector;
	FRotator DestOrientation = FRotator::ZeroRotator;

	ATeleFrag* TeleFrag;
};
