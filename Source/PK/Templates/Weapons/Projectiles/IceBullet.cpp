// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "IceBullet.h"
#include "Main/Util.h"


// Sets default values
AIceBullet::AIceBullet(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicateMovement = false;

	GetCollisionComp()->InitSphereRadius(10.0f);

	DefaultProjectileSpeed = 4200.f; // 50 * 84
	GetProjectileMovement()->InitialSpeed = DefaultProjectileSpeed;
	GetProjectileMovement()->MaxSpeed = DefaultProjectileSpeed;

	AmmoType = 0x08;
	DamageType = UIcebulletDamage::StaticClass();

	static ConstructorHelpers::FObjectFinder<UParticleSystem> Particle(TEXT("ParticleSystem'/Game/Particles/Trail_IceBullet.Trail_IceBullet'"));
	RocketTrail = Particle.Object;
	static ConstructorHelpers::FObjectFinder<UParticleSystem> HitParticle(TEXT("ParticleSystem'/Game/Particles/IceBulletHit.IceBulletHit'"));
	IceBulletHit = HitParticle.Object;

	FreezerShoot = UTIL.GetSound("weapons/shotgun", "freezer-shoot");
	ExplosionSnd = UTIL.GetSound("impacts", "bullet-glass1");

	Damage = SMALL_NUMBER; // nominal damage for a function call to occur
	// Deactivate after 1 second
	TimeToLive = 1.0f;
}

// Called when the game starts or when spawned
void AIceBullet::BeginPlay()
{
	Super::BeginPlay();

	pscL = UGameplayStatics::SpawnEmitterAttached(RocketTrail, GetCollisionComp(), NAME_None, FVector(0, -10, 0));
	pscR = UGameplayStatics::SpawnEmitterAttached(RocketTrail, GetCollisionComp(), NAME_None, FVector(0,  10, 0));

	if (b3rdPerson && GetOwner())
		UGameplayStatics::PlaySoundAttached(FreezerShoot, GetOwner()->GetRootComponent());
}

// Called every frame
void AIceBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AIceBullet::OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::OnHit(OtherActor, OtherComp, NormalImpulse, Hit);
}

void AIceBullet::OnBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool fromSweep, const FHitResult& Hit)
{
	Super::OnBeginOverlap(OtherActor, OtherComp, OtherBodyIndex, fromSweep, Hit);
}

void AIceBullet::Deactivate_Implementation()
{
	Super::Deactivate_Implementation();

	if (pscL) pscL->SetActive(false);
	if (pscL) pscR->SetActive(false);
	
	FVector L = GetActorRotation().RotateVector(FVector(-20, -20, 0));
	FVector R = GetActorRotation().RotateVector(FVector(-20,  20, 0));	

	UGameplayStatics::SpawnEmitterAttached(IceBulletHit, GetRootComponent(), NAME_None, GetActorLocation() + L, GetActorRotation(), EAttachLocation::KeepWorldPosition);
	UGameplayStatics::SpawnEmitterAttached(IceBulletHit, GetRootComponent(), NAME_None, GetActorLocation() + R, GetActorRotation(), EAttachLocation::KeepWorldPosition);

	UGameplayStatics::PlaySoundAtLocation(this, ExplosionSnd, GetActorLocation());
}