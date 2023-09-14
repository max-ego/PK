// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "Grenade.h"
#include "UnrealNetwork.h"
#include "Templates/Weapons/Projectiles/Stake.h"


// Sets default values
AGrenade::AGrenade(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicateMovement = true;

	DefaultProjectileSpeed = 2000.f;
	GetProjectileMovement()->InitialSpeed = 0;
	GetProjectileMovement()->MaxSpeed = 0;

	GetProjectileMovement()->ProjectileGravityScale = 0.f;
	GetProjectileMovement()->bRotationFollowsVelocity = true;

	GetProjectileMovement()->Bounciness = 0.85f;

	ExplosionSnd = UTIL.GetSound("weapons/grenadelauncher", "weapon_grenade_explosion");

	RocketType = ERocketType::grenade;

	DamageType = UGrenadeDamage::StaticClass();

	Damage = 110.f;
}

void AGrenade::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner() && GetOwner() == Instigator){
		if (HasAuthority() && !IsSpawnedBehindWall()){
			ACharacter* Pawn = Cast<ACharacter>(GetOwner()); // ANetPlayer
			FVector Vel = Pawn->GetCharacterMovement()->Velocity;
			FVector FV = Pawn->GetBaseAimRotation().RotateVector(FVector::ForwardVector);
			Vel = FVector(Vel.X * FMath::Abs(FV.X) * 0.6f, Vel.Y * FMath::Abs(FV.Y) * 0.6f, Vel.Z * FMath::Abs(FV.Z) / 2 * 0.7f);
			AddVelocity = FV * DefaultProjectileSpeed + Vel + FVector(0, 0, 800);
			OnRep_AddVelocity();
		}
	}
	else{
		Damage = 200.f; // combo damage
		if (!GetOwner()) SetDeactivateTimer(0.016);
		else SetDeactivateTimer(0);
		FVector FV = GetActorRotation().RotateVector(FVector::ForwardVector);
		GetProjectileMovement()->Velocity = FV * 1500.f;
	}
}

// Called every frame
void AGrenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (BounceSoundTimeOut > 0) BounceSoundTimeOut -= DeltaTime;
}

void AGrenade::OnRep_BounceSnd()
{
	UGameplayStatics::PlaySoundAtLocation(this, grenade_bounce, GetActorLocation());
}

void AGrenade::OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!HasAuthority()) return;
	
	if (OtherActor && OtherActor->IsA(AStake::StaticClass()))
	{
		OtherActor = NULL;
	}
	
	if (GetOwner() && GetOwner()->IsA(AStake::StaticClass()))
	{
		Super::OnHit(OtherActor, OtherComp, NormalImpulse, Hit);
	}
	else{
		if (GetOwner() && GetOwner()->IsA(ACharacter::StaticClass())) bHitOwner = true; // hit everyone after bounce

		if (BounceSoundTimeOut <= 0){
			bBounceSnd++; // notify client of the bounce
			UGameplayStatics::PlaySoundAtLocation(this, grenade_bounce, GetActorLocation());
			BounceSoundTimeOut = 0.2f;
		}
	}
}

void AGrenade::OnBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool fromSweep, const FHitResult& Hit)
{
	if (!HasAuthority()) return;
	
	if (OtherActor && OtherActor->IsA(AStake::StaticClass()))
	{
		OtherActor = NULL;
	}
	Super::OnBeginOverlap(OtherActor, OtherComp, OtherBodyIndex, fromSweep, Hit);
}

void AGrenade::Deactivate_Implementation()
{
	Super::Deactivate_Implementation();
}

void AGrenade::DeactivateQuiet_Implementation()
{
	APKProjectile::Deactivate_Implementation();
}

bool AGrenade::IsSpawnedBehindWall()
{
	AActor* Owner = GetOwner();
	FHitResult Hit = FHitResult(1.f);
	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("Trace")), true, Owner);
	const FVector Start = Owner ? Owner->GetActorLocation() : GetActorLocation();
	const FVector End = GetActorLocation();

	GetWorld()->LineTraceSingle(Hit, Start, End, ECollisionChannel::ECC_Visibility, TraceParams);

	if (Hit.bBlockingHit)
	{
		Deactivate();
		return true;
	}

	return false;
}

void AGrenade::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGrenade, bBounceSnd);
}