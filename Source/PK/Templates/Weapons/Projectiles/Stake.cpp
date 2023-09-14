// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "Stake.h"
#include "Templates/Weapons/Projectiles/Grenade.h"
#include "StickedStake.h"
#include "Main/Util.h"

#include "Misc/DefaultValueHelper.h"


// Sets default values
AStake::AStake(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	// dummy grenade
	StaticMeshComponentGrenade = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshGrenade"));
	StaticMeshComponentGrenade->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	StaticMeshComponentGrenade->bGenerateOverlapEvents = false;
	StaticMeshComponentGrenade->Mobility = EComponentMobility::Movable;
	StaticMeshComponentGrenade->AttachParent = GetCollisionComp();
	StaticMeshComponentGrenade->SetHiddenInGame(true);

	static ConstructorHelpers::FObjectFinder<UStaticMesh>GrenadeMesh(TEXT("/Game/Items/granat.granat"));
	StaticMeshComponentGrenade->SetStaticMesh(GrenadeMesh.Object);
	StaticMeshComponentGrenade->SetRelativeLocation(FVector(-20.0f, 0.0f, 0.0f));
	StaticMeshComponentGrenade->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));

	PrimaryActorTick.bCanEverTick = true;

	bReplicateMovement = true;

	GetCollisionComp()->InitSphereRadius(111.0f);
	GetCollisionComp()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetCollisionComp()->BodyInstance.SetCollisionProfileName("OverlapAll");

	static ConstructorHelpers::FObjectFinder<UStaticMesh>ProjectileMesh(TEXT("StaticMesh'/Game/Items/kolek.kolek'"));
	GetStaticMeshComp()->SetStaticMesh(ProjectileMesh.Object);
	GetStaticMeshComp()->SetRelativeLocation(FVector(-100.0f, 0.0f, 0.0f));
	GetStaticMeshComp()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	DefaultProjectileSpeed = 5880.f;
	GetProjectileMovement()->InitialSpeed = DefaultProjectileSpeed;
	GetProjectileMovement()->MaxSpeed = DefaultProjectileSpeed;

	GetProjectileMovement()->ProjectileGravityScale = 0.6f;
	GetProjectileMovement()->bRotationFollowsVelocity = true;

	AmmoType = 0x01;
	DamageType = UStakeDamage::StaticClass();

	static ConstructorHelpers::FObjectFinder<UParticleSystem> Particle(TEXT("ParticleSystem'/Game/Particles/Trail_Rocket.Trail_Rocket'"));
	RocketTrail = Particle.Object;

	static ConstructorHelpers::FObjectFinder<UObject>StakeShieldCue(TEXT("SoundCue'/Game/Sounds/impacts/Cue/stake-shield.stake-shield'"));
	StakeShield = (USoundCue*)StakeShieldCue.Object;
	static ConstructorHelpers::FObjectFinder<UObject>StakeDefaultCue(TEXT("SoundCue'/Game/Sounds/impacts/Cue/stake-default.stake-default'"));
	StakeDefault = (USoundCue*)StakeDefaultCue.Object;

	StakeFire = UTIL.GetSound("weapons/stake", "weapon_stake-shoot_mp");
	static ConstructorHelpers::FObjectFinder<UObject>StakeOnfly(TEXT("SoundCue'/Game/Sounds/weapons/stake/Cue/StakeOnflyLoop.StakeOnflyLoop'"));
	StakeOnflyLoop = (USoundCue*)StakeOnfly.Object;

	StakeHitComboSnd = UTIL.GetSound("weapons/grenadelauncher", "weapon_grenade_explosion");
	StakeFireComboSnd = UTIL.GetSound("weapons/stake", "stake-fire-combo");

	Damage = 200.f;

	TimeToLive = CalcTimeToLive();
}

float AStake::CalcTimeToLive()
{
	FString DefaultGravityZ;
	GConfig->GetString(
		TEXT("/Script/Engine.PhysicsSettings"),
		TEXT("DefaultGravityZ"),
		DefaultGravityZ,
		GEngineIni
		);
	float GravityZ = 0.0f;
	FDefaultValueHelper::ParseFloat(DefaultGravityZ, GravityZ);
	
	return 2.f * (DefaultProjectileSpeed / -(GetProjectileMovement()->ProjectileGravityScale * GravityZ));
}

// Called when the game starts or when spawned
void AStake::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(BurnStakeTimerHandle, this, &AStake::BurnStake, 0.35, false);
	
	Owner = Cast<ANetPlayer>(GetOwner());
	Start = Owner ? Owner->GetActorLocation() : GetActorLocation();

	Trail = UGameplayStatics::SpawnEmitterAttached(RocketTrail, GetCollisionComp(), NAME_None, FVector(-200.0f, 0.0f, 0.0f));

	if (b3rdPerson)
	{
		if (Owner) UGameplayStatics::PlaySoundAttached(StakeFire, Owner->GetRootComponent());
	}
	
	// workaround:
	// client keeps playing 'StakeOnflyLoop' sound when stake is
	// deactivated immediately after spawning (spawned in geometry).
	// Possibly due to delayed playing of SoundCue on client.
	if (Owner){
		HitScan();
		if (!HitResult.bBlockingHit)
		{
			UTIL.PlaySnd(AudioComponent, StakeOnflyLoop);
		}
	}
}

// Called every frame
void AStake::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority() && Owner && Owner->IsValidLowLevel() && !Owner->IsPendingKill()) HitScan();
}

void AStake::HitScan()
{
	FHitResult Hit = FHitResult(1.f);
	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("Trace")), true, Owner); // could crash on match finish
	const FVector End = GetActorLocation();

	GetWorld()->LineTraceSingle(Hit, Start, End, ECollisionChannel::ECC_Visibility, TraceParams);

	if (Hit.bBlockingHit)
	{
		HitResult = Hit;
		if (!bCombo)
		{
			if (Role == ROLE_Authority) ApplyDamage(Hit.GetActor());

			ACharacter* enemy = Cast<ACharacter>(Hit.GetActor());
			if (enemy)
			{
				UPKCharacterMovementComponent* CharacterMovement = Cast<UPKCharacterMovementComponent>(enemy->GetCharacterMovement());
				if (CharacterMovement)
				{
					CharacterMovement->ReceiveImpactRPC(2000 * GetActorRotation().RotateVector(FVector(1.0f, 0.0f, 0.0f)));
				}
			}
			else{
				Owner = NULL; // see AStickedStake::BeginPlay()
			}
		}
		Deactivate();
	}
	else{
		Start = End;
	}
}

void AStake::OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::OnHit(OtherActor, OtherComp, NormalImpulse, Hit);
}

void AStake::OnBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool fromSweep, const FHitResult& Hit)
{
	if (HasAuthority() && OtherActor)
	{
		if (OtherActor->IsA(AGrenade::StaticClass())) {
			if (!bCombo)
			{
				Cast<AGrenade>(OtherActor)->DeactivateQuiet(); // keeps trail particle
				ComboFireFX();
			}
		}
		else if (OtherActor->IsA(ARocket::StaticClass()))
		{
			FVector FlightLine = GetActorRotation().RotateVector(FVector::ForwardVector);
			float d = FMath::PointDistToLine(OtherActor->GetActorLocation(), FlightLine, GetActorLocation());

			if (d < 30)
			{
				OnMissileCollision();
				Cast<APKProjectile>(OtherActor)->Deactivate();
			}
		}
	}	
}

void AStake::ComboFireFX_Implementation()
{
	bCombo = true;
	if (Owner && Owner->IsValidLowLevel() && !Owner->IsPendingKill())
		UGameplayStatics::PlaySoundAttached(StakeFireComboSnd, Owner->GetRootComponent());
	else
		UGameplayStatics::PlaySoundAtLocation(this, StakeFireComboSnd, GetActorLocation());

	StaticMeshComponentGrenade->SetHiddenInGame(false);
}

void AStake::BurnStake()
{
	Damage = 250.f;
}

void AStake::Deactivate_Implementation()
{
	SetActorTickEnabled(false);
	GetProjectileMovement()->bRotationFollowsVelocity = false;
	AudioComponent->Stop();
	StaticMeshComponentGrenade->SetHiddenInGame(true);

	Super::Deactivate_Implementation();
	
	if (bCombo || bMissileCollision) UTIL.PlaySnd(AudioComponent, StakeShield);

	if (HasAuthority())
	{
		FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
		SpawnParameters.Instigator = Owner;

		FVector Location = HitResult.bBlockingHit ? HitResult.ImpactPoint : GetActorLocation();

		if (bCombo)
		{
			TSubclassOf<AGrenade> ProjectileCls = AGrenade::StaticClass();
			
			FVector Offset = GetActorRotation().RotateVector(FVector(-200.0f, 0.0f, 0.0f));
			const FVector SpawnLocation = GetActorLocation() + Offset;
			const FRotator SpawnRotation = GetActorRotation();
			
			if (HitResult.bBlockingHit) SpawnParameters.Owner = this;
			GrenadeCombo = GetWorld()->SpawnActor<AGrenade>(ProjectileCls, SpawnLocation, SpawnRotation, SpawnParameters);
		}
		else if (HitResult.bBlockingHit)
		{
			SpawnParameters.Owner = HitResult.GetActor()/*Owner*/;
			SpawnParameters.Instigator = Owner;
			APKProjectile* Stick = GetWorld()->SpawnActor<AStickedStake>(
				HitResult.ImpactPoint + GetActorRotation().RotateVector(FVector(66.6f, 0.0f, 0.0f)),
				GetActorRotation(), SpawnParameters);
		}
	}
}