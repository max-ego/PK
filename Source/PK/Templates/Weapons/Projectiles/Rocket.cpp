// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "Rocket.h"
#include "Main/Util.h"
#include "PKClasses/Explosion.h"
#include "UnrealNetwork.h"

float ARocket::ExplosionRange = 600.f;

// Sets default values
ARocket::ARocket(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
 	PrimaryActorTick.bCanEverTick = false;

	bReplicateMovement = false;
	
	GetCollisionComp()->InitSphereRadius(10.0f);
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh>ProjectileMesh(TEXT("/Game/Items/granat.granat"));
	GetStaticMeshComp()->SetStaticMesh(ProjectileMesh.Object);
	GetStaticMeshComp()->SetRelativeLocation(FVector(-20.0f, 0.0f, 0.0f));
	GetStaticMeshComp()->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	
	DefaultProjectileSpeed = 4200.f; /*3360*/
	GetProjectileMovement()->InitialSpeed = DefaultProjectileSpeed;
	GetProjectileMovement()->MaxSpeed = DefaultProjectileSpeed;
	
	AmmoType = 0x02;
	DamageType = URocketDamage::StaticClass();
	
	static ConstructorHelpers::FObjectFinder<UParticleSystem> Particle(TEXT("ParticleSystem'/Game/Particles/Trail_Rocket.Trail_Rocket'"));
	RocketTrail = Particle.Object;

	ExplosionSnd = UTIL.GetSound("weapons/machinegun", "rocket_hit");
	rl_shoot = UTIL.GetSound("weapons/machinegun", "rl_shoot");
	grenade_fire = UTIL.GetSound("weapons/grenadelauncher", "weapon_grenade_fire");
	grenade_bounce = UTIL.GetSound("weapons/grenadelauncher", "weapon_grenade_bounce");

	static ConstructorHelpers::FObjectFinder<UObject>RocketFly(TEXT("SoundCue'/Game/Sounds/weapons/machinegun/Cue/RocketFlyingNoise.RocketFlyingNoise'"));
	RocketFlyingNoiseLoop = (USoundCue*)RocketFly.Object;


	AttenuationSettings = CreateDefaultSubobject<USoundAttenuation>(TEXT("AExplosionSoundAttenuation"));
	AttenuationSettings->Attenuation.DistanceAlgorithm = ESoundDistanceModel::ATTENUATION_NaturalSound;
	AttenuationSettings->Attenuation.dBAttenuationAtMax = -45.f;
	AttenuationSettings->Attenuation.OmniRadius = 4000.f;
	AttenuationSettings->Attenuation.FalloffDistance = 6000.f;

	if (!(WITH_EDITOR || IsRunningDedicatedServer())) {
		ExplosionSnd->AttenuationSettings = AttenuationSettings;
		rl_shoot->AttenuationSettings = AttenuationSettings;
	}

	// Deactivate after 2 seconds
	TimeToLive = 2.0f;

	Damage = 100.f;
}

// Called when the game starts or when spawned
void ARocket::BeginPlay()
{
	Super::BeginPlay();
	UGameplayStatics::SpawnEmitterAttached(
		RocketTrail,
		GetRootComponent()
		);

	switch (RocketType){
	case ERocketType::rocket:
		UTIL.PlaySnd(AudioComponent, RocketFlyingNoiseLoop);
		if (b3rdPerson && GetOwner())/*UGameplayStatics::PlaySoundAtLocation(this, rl_shoot, GetActorLocation());*/
			UGameplayStatics::PlaySoundAttached(rl_shoot, GetOwner()->GetRootComponent());
		break;
	case ERocketType::grenade:
		if (b3rdPerson && GetOwner() && GetOwner()->IsA(ACharacter::StaticClass()))/*UGameplayStatics::PlaySoundAtLocation(this, grenade_fire, GetActorLocation());*/
			UGameplayStatics::PlaySoundAttached(grenade_fire, GetOwner()->GetRootComponent());
		break;
	default:
		break;
	}
}

// Called every frame
void ARocket::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ARocket::OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::OnHit(OtherActor, OtherComp, NormalImpulse, Hit);
}

void ARocket::OnBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool fromSweep, const FHitResult& Hit)
{
	Super::OnBeginOverlap(OtherActor, OtherComp, OtherBodyIndex, fromSweep, Hit);
}

void ARocket::Deactivate_Implementation()
{
	if (!bIsActive) return;
	Super::Deactivate_Implementation();

	AudioComponent->Stop();

	if (HasAuthority()) {
		FVector_NetQuantize10 Loc = GetActorLocation();
		FVector_NetQuantize10 Rot = RocketType == ERocketType::rocket ? GetActorRotation().Vector() : FVector(0, 0, 1);
		bool bHitMovable = false;
		if (HitResult.Actor != NULL && HitResult.Actor != this) {
			Loc = HitResult.ImpactPoint;
			Rot = HitResult.ImpactNormal;
		}
		FVector_NetQuantize10 locrot[] = { Loc, Rot };
		ExplosionLocRot.Append(locrot, ARRAY_COUNT(locrot));
		OnRep_ExplosionLoc();
	}
}

void ARocket::ApplyDamage(AActor* OtherActor, TSubclassOf<UPKDamageType> DamageTypeClass)
{
	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("Trace")), true, this);
	TArray<ANetPlayer*> Pawns;

	UTIL.FindAllObjectsClassOf(Pawns);
	for (auto pawn : Pawns)
	{
		if (pawn->PlayerState)
		{
			bool bBlockingHit = GetWorld()->LineTraceTestByProfile(GetActorLocation(), pawn->GetActorLocation(), TEXT("Spectator"), TraceParams);
			float dist = (GetActorLocation() - pawn->GetActorLocation()).Size();
			
			if (bBlockingHit || dist > ExplosionRange) continue;
			float ImpactRatio = 1.f - FMath::Pow(dist / ExplosionRange, 4);

			if (PC) UGameplayStatics::ApplyDamage(pawn, Damage * ImpactRatio, PC, this, DamageType);
		}		
	}
}

FVector ARocket::GetExplosionVelocity(FVector ExplosionLoc, UMovementComponent* ActorMovementComp)
{
	float ExplosionStrength = 550.f;
	float JumpZVelocity = 0.f;
	FVector Vel = FVector::ZeroVector;

	float dist = (ExplosionLoc - ActorMovementComp->UpdatedComponent->GetComponentLocation()).Size();
	if (dist > ExplosionRange) return Vel;

	FVector ExplosionToActor = (ActorMovementComp->UpdatedComponent->GetComponentLocation() - ExplosionLoc).GetSafeNormal();
	
	float ImpactRatio = 1.f - FMath::Pow(dist / ExplosionRange, 4);
	if (ActorMovementComp->IsA(UPKCharacterMovementComponent::StaticClass()))
	{
		UPKCharacterMovementComponent* CharacterMovement = Cast<UPKCharacterMovementComponent>(ActorMovementComp);
		if (CharacterMovement->IsFalling()) ImpactRatio *= 1.2f;
		JumpZVelocity = CharacterMovement->JumpZVelocity;
	}

	float VelZ = JumpZVelocity;
	/*eliminate negative vertical velocity*/
	if (ActorMovementComp->Velocity.Z < 0) VelZ -= ActorMovementComp->Velocity.Z;	
	
	Vel = (ActorMovementComp->Velocity.GetSafeNormal2D() + ExplosionToActor).GetSafeNormal() * ExplosionStrength * ImpactRatio;
	Vel.Z += VelZ * ImpactRatio;

	return Vel;
}

void ARocket::OnRep_ExplosionLoc()
{
	FVector SpawnLocation = ExplosionLocRot[0];
	FRotator SpawnRotation = ExplosionLocRot[1].Rotation();

	FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
	SpawnParameters.Instigator = Instigator;
	SpawnParameters.Owner = this;

	GetWorld()->SpawnActor<AExplosion>(AExplosion::StaticClass(), SpawnLocation, SpawnRotation, SpawnParameters);
	UGameplayStatics::PlaySoundAtLocation(this, ExplosionSnd, SpawnLocation);

	if (IsRunningDedicatedServer() || !(GetOwner() && GetOwner()->IsValidLowLevel() && !GetOwner()->IsPendingKill())) return;

	/*apply explosion impulse*/
	UPKGameInstance* GI = Cast<UPKGameInstance>(GetWorld()->GetGameInstance());
	ANetPlayer* player = Cast<ANetPlayer>(GI->GetFirstLocalPlayerController()->AcknowledgedPawn);

	if (player && (player != GetOwner() || RocketType == ERocketType::grenade))
	{
		/*check for obstacles*/
		FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("Trace")), true, this);
		bool bBlockingHit = GetWorld()->LineTraceTestByProfile(GetActorLocation(), player->GetActorLocation(), TEXT("Spectator"), TraceParams);

		if (!bBlockingHit)
		{
			UPKCharacterMovementComponent* CharacterMovement = Cast<UPKCharacterMovementComponent>(player->GetCharacterMovement());
			CharacterMovement->ReceiveImpact(ARocket::GetExplosionVelocity(SpawnLocation, CharacterMovement));
		}
	}
}

void ARocket::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARocket, ExplosionLocRot);
}