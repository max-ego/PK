// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "MiniGunFire.h"
#include "Main/Util.h"
#include "Main/NetPlayerController.h"
#include "PKClasses/PKWeapon.h"


// Sets default values
AMiniGunFire::AMiniGunFire(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bReplicateMovement = true;

	GetCollisionComp()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	PrimaryActorTick.bCanEverTick = true;
	
	DefaultProjectileSpeed = 0.f;
	GetProjectileMovement()->InitialSpeed = DefaultProjectileSpeed;
	GetProjectileMovement()->MaxSpeed = DefaultProjectileSpeed;
	AmmoType = 0x10;
	SpecificDamageType = UMinigunDamage::StaticClass();

	static ConstructorHelpers::FObjectFinder<UParticleSystem> MinigunParticle(TEXT("/Game/Particles/MinigunHitWall.MinigunHitWall"));
	MinigunHitWallParticle = MinigunParticle.Object;
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ShotgunParticle(TEXT("/Game/Particles/shotgunhitwall.shotgunhitwall"));
	ShotgunHitWallParticle = ShotgunParticle.Object;

	static ConstructorHelpers::FObjectFinder<UObject>MgunBullets(TEXT("SoundCue'/Game/Sounds/weapons/machinegun/Cue/MgunBullets.MgunBullets'"));
	Bullets = (USoundCue*)MgunBullets.Object;
	MGunStop = UTIL.GetSound("weapons/machinegun", "rotor-stop");

	// let the particle end
	DeactivatedLifeSpan = 1.0f;

	Damage = 0;
}

// Called when the game starts or when spawned
void AMiniGunFire::BeginPlay()
{
	Super::BeginPlay();

	Owner = Cast<ANetPlayer>(GetOwner());

	if (b3rdPerson) UTIL.PlaySnd(AudioComponent, Bullets);
}

// Called every frame
void AMiniGunFire::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!(Owner && Owner->IsValidLowLevel() && !Owner->IsPendingKill()))
	{
		if (HasAuthority() && IsValidLowLevel() && !IsPendingKill()) Destroy();
		return;
	}

	if (HasAuthority())
	{
		const FRotator Rotation = Owner->GetBaseAimRotation();
		FVector Loc = Owner->GetFirstPersonCameraComponent()->GetComponentLocation();
		const FVector Location = Loc + Rotation.RotateVector(Owner->GunOffset);
		SetActorLocationAndRotation(Location, Rotation);
	}

	if (!bHitScanEnabled) return;

	HitScan(DeltaTime);
	ApplySpecificDamage(AltFireDamage, DeltaTime);

	if (HasAuthority() && Owner->Controller)
	{
		if (BulletTimeout > 0) BulletTimeout -= DeltaTime;
		else{
			if (PC)
			{
				if (PC->GetAmmo(AmmoType) == 0)
				{
					StopFire();
				}
				else
				{
					PC->ConsumeAmmo(AmmoType);
					BulletTimeout = 0.2;
				}
			}
		}
	}
}

void AMiniGunFire::HitScan(float DeltaTime)
{
	const FRotator StartRotation = GetActorRotation();
	const FVector StartLocation = GetActorLocation();

	float MinigunFireRange = 100.f * 84.f;
	const FVector End = StartLocation + MinigunFireRange * StartRotation.RotateVector(FVector::ForwardVector);

	FHitResult Hit = FHitResult(1.f);
	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("Trace")), true, Owner);

	GetWorld()->LineTraceSingle(Hit, StartLocation, End, ECollisionChannel::ECC_Visibility, TraceParams);

	if (Hit.bBlockingHit)
	{
		if (HasAuthority())
		{
			if (Hit.GetActor()->IsA(ANetPlayer::StaticClass()) ||
				Hit.GetActor()->IsA(ADestructibleItemBase::StaticClass()))
			{
				if (Owner->MinigunLastHitEnemy != Hit.Actor)
				{
					ApplyRemainingDamage();

					Enemy = Hit.GetActor(); if (PC && Hit.GetActor()->IsA(ANetPlayer::StaticClass())) PC->bSpecificHitSound = true;
					if (Cast<ANetPlayer>(Enemy))
					{
						bHitEnemy = true;
						Cast<ANetPlayer>(Enemy)->MinigunAttackingPlayerFlags |= Owner->ClearMinigunLastHitEnemy();
						Owner->MinigunLastHitEnemy = Hit.Actor;
					}
				}
			}
			else
			{
				ApplyRemainingDamage();
				Owner->ClearMinigunLastHitEnemy();
			}
		}

		if (ParticleTimeout <= 0){
			if (bHitEnemy)
			{
				// TODO: hit enemy particle and sound
			}
			else
			{
				if (!ParticlesPerHitSound)
				{
					// hit wall sound
					USoundBase* HitSnd = UTIL.GetSound("impacts", "bullet-stone");
					uint8 snd = FMath::RandRange(0, 2);
					switch (snd){
					case 0:
						HitSnd = UTIL.GetSound("impacts", FString::Printf(TEXT("bullet-stone%d"), FMath::RandRange(2, 4)));
						break;
					case 1:
						HitSnd = UTIL.GetSound("impacts", FString::Printf(TEXT("ricochet%d"), FMath::RandRange(1, 4)));
						break;
					case 2:
						HitSnd = UTIL.GetSound("impacts", "bullet-stone");
						break;
						/*default:
							HitSnd = UTIL.GetSound("impacts", "bullet-stone");*/
					}
					UGameplayStatics::PlaySoundAtLocation(this, HitSnd, Hit.ImpactPoint);
				}
				if (ParticlesPerHitSound > 3) ParticlesPerHitSound = 0;
				else ParticlesPerHitSound++;
				
				// 'sparks' particle
				UGameplayStatics::SpawnEmitterAttached(
					MinigunHitWallParticle,
					GetRootComponent(),
					NAME_None,
					Hit.ImpactPoint,
					Hit.ImpactNormal.Rotation(),
					EAttachLocation::KeepWorldPosition,
					true
					);
			}			
			ParticleTimeout = 0.033f;
		}
	}
	else
	{
		ApplyRemainingDamage();
		Owner->ClearMinigunLastHitEnemy();
	}
}

void AMiniGunFire::Deactivate_Implementation()
{
	Super::Deactivate_Implementation();

	AudioComponent->Stop();
	
	if (HasAuthority()) Owner->ClearMinigunLastHitEnemy();
	
	if (b3rdPerson && GetOwner())
		UGameplayStatics::PlaySoundAttached(MGunStop, GetOwner()->GetRootComponent());
}

void AMiniGunFire::StopFire_Implementation()
{
	ANetPlayer* player = Cast<ANetPlayer>(GetOwner());

	if (player) {
		APKWeapon* cw = Cast<APKWeapon>(player->Weapons[player->CurWeaponIndex - 1]->ChildActor);
		if (cw) cw->OnFinishAltFire();
	}
}