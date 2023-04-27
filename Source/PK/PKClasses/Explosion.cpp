// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "Explosion.h"
#include "PKProjectile.h"
#include "PKClasses/DamageTypes/ExplosionDamage.h"
#include "Templates/Weapons/Projectiles/Rocket.h"
#include "PKClasses/DestructibleItemBase.h"


// Sets default values
AExplosion::AExplosion(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = Scene;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	static ConstructorHelpers::FObjectFinder<UParticleSystem> Particle(TEXT("/Game/Particles/Explosion.Explosion"));
	ExplosionParticle = Particle.Object;
	
	RocketExplosionSnd = UTIL.GetSound("weapons/machinegun", "rocket_hit");
	GrenadeComboExplosionSnd = UTIL.GetSound("weapons/grenadelauncher", "weapon_grenade_explosion");

	DamageType = UExplosionDamage::StaticClass();

	ExplosionRange = ARocket::ExplosionRange/*600.f*/;

}

// Called when the game starts or when spawned
void AExplosion::BeginPlay()
{
	Super::BeginPlay();

	UWorld* const World = GetWorld();

	check(World);

	ACharacter* Pawn = Cast<ACharacter>(Instigator);
	if (Pawn) PC = Cast<ANetPlayerController>(Pawn->Controller);

	DoExplosion();
}

// Called every frame
void AExplosion::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AExplosion::DoExplosion()
{
	ENetMode NetMode = GetNetMode();
	if (NetMode == NM_Standalone || NetMode == NM_ListenServer || NetMode == NM_DedicatedServer)
	{
		ApplyDamage();
	}

	UWorld* const World = GetWorld();
	if (World != NULL)
	{
		ARadialForceActor* RadialForceActor = World->SpawnActorDeferred<ARadialForceActor>(ARadialForceActor::StaticClass(), GetActorLocation(), GetActorRotation());
		
		URadialForceComponent* RadialForceComp = RadialForceActor->GetForceComponent();
		RadialForceComp->RemoveObjectTypeToAffect(UEngineTypes::ConvertToObjectType(ECC_Pawn));

		RadialForceComp->AddObjectTypeToAffect(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
		RadialForceComp->DestructibleDamage = 100.f;
		RadialForceComp->ForceStrength = 0.f;
		RadialForceComp->ImpulseStrength = 7000.f;
		RadialForceComp->Radius = 1000.f;

		RadialForceActor->DisableForce();

		UGameplayStatics::FinishSpawningActor(RadialForceActor, RadialForceActor->GetTransform());
		RadialForceActor->FireImpulse();
		RadialForceActor->SetLifeSpan(2.f);
	}
	
	UGameplayStatics::SpawnEmitterAttached(
		ExplosionParticle,
		GetRootComponent(),
		NAME_None,
		GetActorLocation(),
		GetActorRotation(),
		EAttachLocation::KeepWorldPosition,
		true
		);

	SetLifeSpan(2.f);
}

void AExplosion::ApplyDamage()
{
	DamageCauser = Cast<APKProjectile>(GetOwner());
	if (!DamageCauser)
	{
		DamageCauser = this;
	}
	else
	{
		APKProjectile* dc = Cast<APKProjectile>(DamageCauser);

		DamageType = dc->DamageType;
		Damage = dc->GetDamage();
	}

	TArray<ANetPlayer*> Pawns;

	UTIL.FindAllObjectsClassOf(Pawns);
	for (auto pawn : Pawns)
	{
		if (pawn->PlayerState)
		{
			TraceApplyDmg(pawn);
		}
	}

	TArray<ADestructibleItemBase*> Destructibles;
	UTIL.FindAllObjectsClassOf(Destructibles);
	for (auto actor : Destructibles)
	{
		TraceApplyDmg(actor);
	}
}

void AExplosion::TraceApplyDmg(AActor* actor)
{
	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("Trace")), false, actor);

	bool bBlockingHit = GetWorld()->LineTraceTestByProfile(GetActorLocation(), actor->GetActorLocation(), TEXT("Spectator"), TraceParams);
	float dist = (GetActorLocation() - actor->GetActorLocation()).Size();

	if (bBlockingHit || dist > ExplosionRange) return;

	float ImpactRatio = 1.f - FMath::Pow(dist / ExplosionRange, 4);
	UGameplayStatics::ApplyDamage(actor, Damage * ImpactRatio, PC, DamageCauser, DamageType);
}