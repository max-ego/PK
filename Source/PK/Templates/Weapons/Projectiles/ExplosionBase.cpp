// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "ExplosionBase.h"


// Sets default values
AExplosionBase::AExplosionBase(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = false;
	bReplicateMovement = false;

	DefaultProjectileSpeed = 0.f;
	GetProjectileMovement()->InitialSpeed = DefaultProjectileSpeed;
	GetProjectileMovement()->MaxSpeed = DefaultProjectileSpeed;
}

// Called when the game starts or when spawned
void AExplosionBase::BeginPlay()
{
	Super::BeginPlay();

	Process();
}

// Called every frame
void AExplosionBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AExplosionBase::Process()
{
	Destroy();
}