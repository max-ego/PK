// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "StickedStake.h"

// Sets default values
AStickedStake::AStickedStake(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicateMovement = false;

	DefaultProjectileSpeed = 0.f;
	GetProjectileMovement()->InitialSpeed = DefaultProjectileSpeed;
	GetProjectileMovement()->MaxSpeed = DefaultProjectileSpeed;

	GetProjectileMovement()->ProjectileGravityScale = 0.f;
	GetProjectileMovement()->bRotationFollowsVelocity = false;

	TimeToLive = 3.0f;
}

// Called every frame
void AStickedStake::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

// Called when the game starts or when spawned
void AStickedStake::BeginPlay()
{
	APKProjectile::BeginPlay();

	GetProjectileMovement()->StopSimulating(FHitResult(1.f));

	if (Instigator != NULL)
	{
		UTIL.PlaySnd(AudioComponent, StakeShield);
		if (GetStaticMeshComp() != nullptr)
			GetStaticMeshComp()->SetHiddenInGame(true);
	}
	else{
		UTIL.PlaySnd(AudioComponent, StakeDefault);
		AActor* Owner = GetOwner();
		if (Owner && Owner->IsValidLowLevel() && !Owner->IsPendingKill())
			AttachRootComponentToActor(Owner, NAME_None, EAttachLocation::KeepWorldPosition, true);
	}
}

/** called when projectile hits something */
void AStickedStake::OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit){}

void AStickedStake::OnBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool fromSweep, const FHitResult& Hit){}

void AStickedStake::Deactivate_Implementation()
{
	APKProjectile::Deactivate_Implementation();
}

