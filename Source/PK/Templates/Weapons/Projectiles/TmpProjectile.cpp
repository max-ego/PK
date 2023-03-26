// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "TmpProjectile.h"
#include "Main/NetPlayer.h"


// Sets default values
ATmpProjectile::ATmpProjectile(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	GetCollisionComp()->InitSphereRadius(15.0f);

	GetCollisionComp()->BodyInstance.SetCollisionProfileName("OverlapAll");
	GetCollisionComp()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

	static ConstructorHelpers::FObjectFinder<UStaticMesh>ProjectileMesh(TEXT("/Game/FirstPerson/Meshes/FirstPersonProjectileMesh.FirstPersonProjectileMesh"));
	GetStaticMeshComp()->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));
	GetStaticMeshComp()->SetStaticMesh(ProjectileMesh.Object);

	// Players can't walk on it
	GetCollisionComp()->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	GetCollisionComp()->CanCharacterStepUpOn = ECB_No;
	
	GetProjectileMovement()->bRotationFollowsVelocity = true;
	GetProjectileMovement()->bShouldBounce = true;
	GetProjectileMovement()->Bounciness = 0.85;

	DefaultProjectileSpeed = 6000.f;
	GetProjectileMovement()->InitialSpeed = DefaultProjectileSpeed;
	GetProjectileMovement()->MaxSpeed = DefaultProjectileSpeed;

	// Deactivate after 2 seconds
	TimeToLive = 2.0f;

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	DamageType = UDamageType::StaticClass();

	static ConstructorHelpers::FObjectFinder<USoundBase> sound(TEXT("SoundWave'/Game/FirstPerson/Audio/board02.board02'"));
	HitSound = sound.Object;

	AttenuationSettings = CreateDefaultSubobject<USoundAttenuation>(TEXT("ProjectileSoundAttenuation"));
	AttenuationSettings->Attenuation.DistanceAlgorithm = ESoundDistanceModel::ATTENUATION_NaturalSound;
	if (!(WITH_EDITOR || IsRunningDedicatedServer())) HitSound->AttenuationSettings = AttenuationSettings;
}

// Called when the game starts or when spawned
void ATmpProjectile::BeginPlay()
{
	Super::BeginPlay();
		
}

// Called every frame
void ATmpProjectile::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ATmpProjectile::OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::OnHit(OtherActor, OtherComp, NormalImpulse, Hit);

	if (HitSound != NULL && (OtherActor && OtherActor != GetOwner()))
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, Hit.ImpactPoint);
	}

	// Only add impulse and destroy projectile if we hit a physics
	if (OtherActor && OtherActor != GetOwner() && OtherActor != this 
		&& OtherComp && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
		
		if (HasAuthority()) Deactivate();
	}	
}

void ATmpProjectile::OnBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool fromSweep, const FHitResult& Hit)
{
	Super::OnBeginOverlap(OtherActor, OtherComp, OtherBodyIndex, fromSweep, Hit);

	if (OtherActor && GetOwner() && OtherActor != GetOwner())
	{
		FVector HitFromDirection = GetProjectileMovement()->Velocity;
		AController* Instigator = (Cast<ANetPlayer>(GetOwner()))->Controller;
		UGameplayStatics::ApplyPointDamage(OtherActor, 10.f, HitFromDirection, Hit, Instigator, this, DamageType);
		if (OtherActor->IsA(ANetPlayer::StaticClass())){
			if (HasAuthority())
			{
				Deactivate();
			}
		}
	}
}