// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "SGShot.h"
#include "Main/Util.h"

// Sets default values
ASGShot::ASGShot(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	GetCollisionComp()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	
	PrimaryActorTick.bCanEverTick = false;

	bReplicateMovement = false;

	DefaultProjectileSpeed = 0.f;
	GetProjectileMovement()->InitialSpeed = DefaultProjectileSpeed;
	GetProjectileMovement()->MaxSpeed = DefaultProjectileSpeed;
	
	AmmoType = 0x04;
	DamageType = UShellDamage::StaticClass();

	static ConstructorHelpers::FObjectFinder<UParticleSystem> Particle(TEXT("/Game/Particles/shotgunhitwall.shotgunhitwall"));
	ShotgunHitWallParticle = Particle.Object;

	ShotgunShoot = UTIL.GetSound("weapons/shotgun", "weapon_shotgun_shoot5");
	
	ShotTimerDelegate.BindUFunction(this, FName("SpreadShot"));
	
	// need to keep EmitterAttached alive
	DeactivatedLifeSpan = 3.f;

	Damage = 1.f;
}

// Called when the game starts or when spawned
void ASGShot::BeginPlay()
{
	Super::BeginPlay();

	SpreadShot();

	if (b3rdPerson && GetOwner())
		UGameplayStatics::PlaySoundAttached(ShotgunShoot, GetOwner()->GetRootComponent());
}

// Called every frame
void ASGShot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASGShot::SpreadShot()
{
	const FRotator StartRotation = GetActorRotation();
	const FVector StartLocation = GetActorLocation();

	float FireRange = 50.f * 84.f;
	uint8 HowManyPellets = 24;
	float Y; float Z; float FireRadius = 4.f * 84.f;
	TMap<uint16, ANetPlayer*> Enemies;

	if (!(GetOwner() && GetOwner()->IsValidLowLevel() && !GetOwner()->IsPendingKill()))
	{
		if (HasAuthority() && IsValidLowLevel() && !IsPendingKill()) Destroy();
		return;
	}
	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("Trace")), true, GetOwner());

	for (int i = 0; i < HowManyPellets; i++)
	{
		UTIL.RadiusRandom2D(FireRadius, Y, Z);
		FVector End = StartLocation + StartRotation.RotateVector(FVector(1.f * FireRange, Y, Z));

		FHitResult Hit = FHitResult(1.f);

		GetWorld()->LineTraceSingle(Hit, StartLocation, End, ECollisionChannel::ECC_Visibility, TraceParams);

		if (Hit.bBlockingHit)
		{
			if (Hit.GetActor()->IsA(ANetPlayer::StaticClass()))
			{
				if (Role == ROLE_Authority)
				{
					ANetPlayer* Enemy = Cast<ANetPlayer>(Hit.GetActor());
					uint16 Flag = Enemy->GetPlayerFlag();

					if (Enemies.Contains(Flag))
					{
						(*(Enemies.Find(Flag)))->Pellets++;
					}
					else{
						Enemy->Pellets++;
						Enemies.Add(Flag, Enemy);
					}
				}
			}
			else{
				if (Role == ROLE_Authority && Hit.GetActor()->IsA(ADestructibleItemBase::StaticClass()))
				{
					ApplyDamage(Hit.GetActor());
				}
				
				UGameplayStatics::SpawnEmitterAttached(
					ShotgunHitWallParticle,
					GetRootComponent(),
					NAME_None,
					Hit.ImpactPoint,
					Hit.ImpactNormal.Rotation(),
					EAttachLocation::KeepWorldPosition,
					true
					);
				
				// hit sound
				uint8 rand = FMath::RandRange(0, 9);
				if (!rand) // 1/10
				{
					rand = FMath::RandRange(0, 1);
					if (rand)
						HitSnd = UTIL.GetSound("impacts", "bullet-ziemia");
					else
						HitSnd = UTIL.GetSound("impacts", FString::Printf(TEXT("bullet-stone%d"), FMath::RandRange(2, 4)));

					UGameplayStatics::PlaySoundAtLocation(this, HitSnd, Hit.ImpactPoint);
				}
			}
		}
	}

	if (Role == ROLE_Authority)
	{
		for (uint8 idx = 0; idx < 16; idx++) // 16 players max
		{
			uint16 Flag = 0x0001 << idx;
			if (Enemies.Contains(Flag))
			{
				ANetPlayer* Enemy = *Enemies.Find(Flag);
				UPKCharacterMovementComponent* CharacterMovement = Cast<UPKCharacterMovementComponent>(Enemy->GetCharacterMovement());
				CharacterMovement->ReceiveImpactRPC(Enemy->Pellets * 110.f * GetActorRotation().RotateVector(FVector::ForwardVector));

				//float Damage = Enemy->Pellets == HowManyPellets ? 75.f : 2.0f * Enemy->Pellets;
				float Damage = 4.0f * Enemy->Pellets;
				if (PC) UGameplayStatics::ApplyDamage(Enemy, Damage, PC, this, DamageType);
				Enemy->Pellets = 0;
			}
		}
		
		/*
		* deferred destruction
		* since immediate Destroy() call prevents 'spread shot' from occurring on client
		*/
		Deactivate();
	}
}

