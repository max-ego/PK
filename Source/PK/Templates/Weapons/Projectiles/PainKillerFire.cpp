// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "PainKillerFire.h"
#include "Main/Util.h"

APainKillerFire::APainKillerFire(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	FVector arr[] = { FVector(1.8, 0, 0), FVector(1.5, 1, 1), FVector(1.5, 1, -1), FVector(1.5, -1, 1), FVector(1.5, -1, -1) };
	Points.Append(arr, ARRAY_COUNT(arr));

	PrimaryActorTick.bCanEverTick = true;
	bReplicateMovement = true;
	
	GetCollisionComp()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	DefaultProjectileSpeed = 0.f;
	GetProjectileMovement()->InitialSpeed = DefaultProjectileSpeed;
	GetProjectileMovement()->MaxSpeed = DefaultProjectileSpeed;
	GetProjectileMovement()->ProjectileGravityScale = 0.f;

	RotorHitEnemy = UTIL.GetSound("weapons/painkiller", "painkiller-hit");
	RotorHitWall = UTIL.GetSound("weapons/painkiller", "painkiller-spoon_hits_geometry");

	static ConstructorHelpers::FObjectFinder<UObject>RotorHitEnemyCue(TEXT("SoundCue'/Game/Sounds/weapons/painkiller/Cue/RotorHitEnemy.RotorHitEnemy'"));
	rotor_hit_enemy = (USoundCue*)RotorHitEnemyCue.Object;
	static ConstructorHelpers::FObjectFinder<UObject>RotorHitWallCue(TEXT("SoundCue'/Game/Sounds/weapons/painkiller/Cue/RotorHitWall.RotorHitWall'"));
	rotor_hit_wall = (USoundCue*)RotorHitWallCue.Object;

	static ConstructorHelpers::FObjectFinder<UObject>RotorStartLoop(TEXT("SoundCue'/Game/Sounds/weapons/painkiller/Cue/RotorStartLoop.RotorStartLoop'"));
	PainRotorStartLoop = (USoundCue*)RotorStartLoop.Object;
	PainRotorStop = UTIL.GetSound("weapons/painkiller", "pain-rotor-stop");
	PainkillerShoot = UTIL.GetSound("weapons/painkiller", "painkiller-shoot");

	PlaySoundTimerDelegate.BindUFunction(this, FName("PlayRotorStopSound"));

	Damage = 0.f;

	AmmoType = 0x80;
	SpecificDamageType = UPainrotorDamage::StaticClass();

	TimeToLive = 0.f; // never die
}

// Called when the game starts or when spawned
void APainKillerFire::BeginPlay()
{
	Super::BeginPlay();

	GetProjectileMovement()->Velocity = FVector::ZeroVector;

	Owner = Cast<ANetPlayer>(GetOwner());

	if (b3rdPerson) UTIL.PlaySnd(AudioComponent, PainRotorStartLoop);
}

// Called every frame
void APainKillerFire::Tick(float DeltaTime)
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
	ApplySpecificDamage(PainKnifeDamage, DeltaTime);
}

void APainKillerFire::HitScan(float DeltaTime)
{
	const FRotator StartRotation = GetActorRotation();
	const FVector StartLocation = GetActorLocation();
	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("Trace")), true, Owner);
	FHitResult Hit = FHitResult(1.f);
	for (auto v : Points){
		FVector End = StartLocation + StartRotation.RotateVector(v * 75.f);
		GetWorld()->LineTraceSingle(Hit, StartLocation, End, ECollisionChannel::ECC_Visibility, TraceParams);
		if (Hit.bBlockingHit){			
			if (HasAuthority()){
				if (Hit.GetActor()->IsA(ACharacter::StaticClass()) ||
					Hit.GetActor()->IsA(ADestructibleItemBase::StaticClass()))
				{
					if (Enemy != Hit.GetActor())
					{
						ApplyRemainingDamage();
						
						Enemy = Hit.GetActor();
						if (PC && Hit.GetActor()->IsA(ANetPlayer::StaticClass()))
						{
							PC->bSpecificHitSound = true;
							bHitEnemy = true;
						}
					}
				}
				else
				{
					ApplyRemainingDamage();
				}
			}

			if (ParticleTimeout <= 0)
			{
				ParticleTimeout = 0.09f;
				if (bHitEnemy)
					UGameplayStatics::PlaySoundAtLocation(this, RotorHitEnemy, GetActorLocation());
				else
				{
					UGameplayStatics::PlaySoundAttached(RotorHitWall, Owner->GetRootComponent());
				}
			}
			break;
		}
		else
		{
			ApplyRemainingDamage();
		}
	}
}

void APainKillerFire::OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	/*Super::OnHit(OtherActor, OtherComp, NormalImpulse, Hit);*/

}

void APainKillerFire::OnBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool fromSweep, const FHitResult& Hit)
{
	/*Super::OnBeginOverlap(OtherActor, OtherComp, OtherBodyIndex, fromSweep, Hit);*/
		
}

void APainKillerFire::Deactivate_Implementation()
{
	Super::Deactivate_Implementation();

	/*PlayRotorStopSound();*/

	GetWorldTimerManager().SetTimer(TimerHandle, this, &APainKillerFire::PlayRotorStopSound, 0.2f, false);
}

void APainKillerFire::PlayRotorStopSound()
{	
	if (b3rdPerson && Owner && Owner->IsValidLowLevel() && !Owner->IsPendingKill()) //A burnt child dreads the fire
	{
		AudioComponent->Stop();
		if (Owner->bPKHeadHide){
			UGameplayStatics::PlaySoundAttached(PainkillerShoot, Owner->GetRootComponent());
		}
		else{
			UGameplayStatics::PlaySoundAttached(PainRotorStop, Owner->GetRootComponent());
		}
	}
}