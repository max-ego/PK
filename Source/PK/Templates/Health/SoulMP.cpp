// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "SoulMP.h"
#include "UnrealNetwork.h"
#include "Main/NetPlayer.h"
#include "Main/NetPlayerController.h"
#include "PKClasses/PickableItem.h"


// Sets default values
ASoulMP::ASoulMP(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));

	NetUpdateFrequency = 10.f;
	bReplicates = true;
	bReplicateMovement = true;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	GetCollisionComp()->BodyInstance.SetCollisionProfileName("Spectator");
	GetCollisionComp()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	GetCollisionComp()->InitSphereRadius(110.0f);
	RootComponent = GetCollisionComp();

	GetCollisionComp()->OnComponentHit.AddDynamic(this, &ASoulMP::OnHit); // set up a notification for when this component hits something blocking
	BeginOverlapDelegate.BindUFunction(this, FName("OnBeginOverlap"));
	GetCollisionComp()->OnComponentBeginOverlap.Add(BeginOverlapDelegate);

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>ProjectileMesh(TEXT("SkeletalMesh'/Game/Models/Items/energy.energy'"));
	GetSkeletalMeshComp()->SetSkeletalMesh(ProjectileMesh.Object);
	GetSkeletalMeshComp()->SetRelativeScale3D(FVector(1.25, 1.25, 1.25));
	static ConstructorHelpers::FObjectFinder<UAnimationAsset>Anim(TEXT("AnimSequence'/Game/Models/Items/energy_idle2.energy_idle2'"));
	GetSkeletalMeshComp()->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	AnimAsset = Anim.Object;
	GetSkeletalMeshComp()->SetAnimation(AnimAsset);	
	GetSkeletalMeshComp()->Mobility = EComponentMobility::Movable;
	GetSkeletalMeshComp()->AttachParent = GetCollisionComp();

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	GetProjectileMovement()->UpdatedComponent = GetCollisionComp();
	GetProjectileMovement()->ProjectileGravityScale = 0.f;
	GetProjectileMovement()->bRotationFollowsVelocity = false;/*avoid PainHead 'sticking' after blocking hit*/
	/*avoid 'sticking' after blocking hit*/
	GetProjectileMovement()->bShouldBounce = true;
	GetProjectileMovement()->Bounciness = 0.f;
	GetProjectileMovement()->BounceVelocityStopSimulatingThreshold = 0.f; // disable StopSimulating()

	DefaultProjectileSpeed = 1500.f;
	GetProjectileMovement()->InitialSpeed = 0;
	GetProjectileMovement()->MaxSpeed = 0;

	FName fnWeapItemSocket = TEXT("rootSocket");

	PKW = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PKW_item"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>PKWMesh(TEXT("SkeletalMesh'/Game/Models/Weapons/PickableItems/PKW/PKW_item.PKW_item'"));
	PKW->SetSkeletalMesh(PKWMesh.Object);
	PKW->AttachTo(GetSkeletalMeshComp(), fnWeapItemSocket, EAttachLocation::SnapToTarget, true);
	PKW->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
	PKW->bHiddenInGame = true;
	
	ASG = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ASG_item"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>ASGMesh(TEXT("SkeletalMesh'/Game/Models/Weapons/PickableItems/ASG/ASG_item.ASG_item'"));
	ASG->SetSkeletalMesh(ASGMesh.Object);
	ASG->AttachTo(GetSkeletalMeshComp(), fnWeapItemSocket, EAttachLocation::SnapToTarget, true);
	ASG->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
	ASG->bHiddenInGame = true;

	KGR = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("KGR_item"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>KGRMesh(TEXT("SkeletalMesh'/Game/Models/Weapons/PickableItems/KGR/KGR_item.KGR_item'"));
	KGR->SetSkeletalMesh(KGRMesh.Object);
	KGR->AttachTo(GetSkeletalMeshComp(), fnWeapItemSocket, EAttachLocation::SnapToTarget, true);
	KGR->SetRelativeRotation(FRotator(0.0f, -105.0f, 0.0f));
	KGR->bHiddenInGame = true;

	CRL = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CRL_item"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>CRLMesh(TEXT("SkeletalMesh'/Game/Models/Weapons/PickableItems/CRL/CRL_item.CRL_item'"));
	CRL->SetSkeletalMesh(CRLMesh.Object);
	CRL->AttachTo(GetSkeletalMeshComp(), fnWeapItemSocket, EAttachLocation::SnapToTarget, true);
	CRL->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
	CRL->bHiddenInGame = true;

	/*ESL = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ESL_item"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>ESLMesh(TEXT("SkeletalMesh'/Game/Models/Weapons/PickableItems/ESL/ESL_item.ESL_item'"));
	ESL->SetSkeletalMesh(ESLMesh.Object);
	ESL->AttachTo(GetSkeletalMeshComp(), fnWeapItemSocket, EAttachLocation::SnapToTarget, true);
	ESL->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
	ESL->bHiddenInGame = true;*/

	WeapItems.Add(PKW);
	WeapItems.Add(ASG);
	WeapItems.Add(KGR);
	WeapItems.Add(CRL);
	/*WeapItems.Add(ESL);*/

	TimeToLive = 30.f;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> Particle(TEXT("ParticleSystem'/Game/Particles/energy/energy_shine.energy_shine'"));
	energy_red = Particle.Object;

	// note: applying AttenuationSettings here causes crash in PIE mode on 'QuitGame'
	class USoundAttenuation* AttenuationSettings = CreateDefaultSubobject<USoundAttenuation>(TEXT("PickItemSoundAttenuation"));
	AttenuationSettings->Attenuation.DistanceAlgorithm = ESoundDistanceModel::ATTENUATION_NaturalSound;
	AttenuationSettings->Attenuation.dBAttenuationAtMax = -45.f;
	AttenuationSettings->Attenuation.OmniRadius = 400.f;

	PickupSound = UTIL.GetSound("multiplayer", "take-backpack");
	if (!(WITH_EDITOR || IsRunningDedicatedServer()))
	{
		if (PickupSound) PickupSound->AttenuationSettings = AttenuationSettings;
	}
}

// Called when the game starts or when spawned
void ASoulMP::BeginPlay()
{
	Super::BeginPlay();

	SetDeactivateTimer(TimeToLive);

	GetSkeletalMeshComp()->PlayAnimation(AnimAsset, true);
	SpawnParticles();

	if (HasAuthority())
	{
		SpawnDir = FVector(0.1f * FMath::RandRange(-3, 3), 0.1f * FMath::RandRange(-3, 3), 1.f);
		OnRep_SpawnDir();
	}

	Owner = Cast<ACharacter>(GetOwner());

	if (Owner)
	{
		ANetPlayerController* PC = Cast<ANetPlayerController>(Owner->Controller);

		if (PC)
		{
			stake = PC->stake;
			grenade = PC->grenade;
			shell = PC->shell;
			icebullet = PC->icebullet;
			bullets = PC->bullets;
		}

		WeapIdx = Cast<ANetPlayer>(Owner)->CurWeaponIndex - 1;
		if (WeapItems[WeapIdx]) WeapItems[WeapIdx]->SetHiddenInGame(false);

		bLoaded = true;
	}	
}

// Called every frame
void ASoulMP::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ASoulMP::GatherCurrentMovement()
{
	Super::GatherCurrentMovement();

	ReplicatedMovement.LinearVelocity = GetProjectileMovement()->Velocity;
}

void ASoulMP::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();

	GetProjectileMovement()->Velocity = ReplicatedMovement.LinearVelocity;
}

void ASoulMP::SpawnParticles()
{
	/*root = UGameplayStatics::SpawnEmitterAttached(energyhealth1, GetSkeletalMeshComp(), FName("root"));*/
	e1 = UGameplayStatics::SpawnEmitterAttached(energy_red, GetSkeletalMeshComp(), FName("e1"));
	e2 = UGameplayStatics::SpawnEmitterAttached(energy_red, GetSkeletalMeshComp(), FName("e2"));
	e3 = UGameplayStatics::SpawnEmitterAttached(energy_red, GetSkeletalMeshComp(), FName("e3"));
	e4 = UGameplayStatics::SpawnEmitterAttached(energy_red, GetSkeletalMeshComp(), FName("e4"));
	e5 = UGameplayStatics::SpawnEmitterAttached(energy_red, GetSkeletalMeshComp(), FName("e5"));
	e6 = UGameplayStatics::SpawnEmitterAttached(energy_red, GetSkeletalMeshComp(), FName("e6"));
}

void ASoulMP::KillParticles()
{
	if (/*root && */e1 && e2 && e3 && e4 && e5 && e6)
	{
		/*root->SetActive(false);*/
		e1->SetActive(false);
		e2->SetActive(false);
		e3->SetActive(false);
		e4->SetActive(false);
		e5->SetActive(false);
		e6->SetActive(false);
	}
}

void ASoulMP::OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
}

void ASoulMP::OnBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool fromSweep, const FHitResult& Hit)
{
	if (!HasAuthority() || !bLoaded) return;

	if (OtherActor && (!Owner || (Owner && OtherActor != Owner)))
	{
		if (OtherActor->IsA(ACharacter::StaticClass()))
		{
			ACharacter* Pawn = Cast<ACharacter>(OtherActor);
			ANetPlayerController* PC = Cast<ANetPlayerController>(Pawn->Controller);
			if (PC)
			{
				PC->AmmoAdd(EAmmoType::megahealth, Health);
				Cast<ANetPlayer>(Pawn)->SelectBestWeapon(0x01 << WeapIdx);

				PC->AmmoAdd(EAmmoType::stake, stake);
				PC->AmmoAdd(EAmmoType::grenade, grenade);
				PC->AmmoAdd(EAmmoType::shell, shell);
				PC->AmmoAdd(EAmmoType::icebullet, 0, icebullet);
				PC->AmmoAdd(EAmmoType::bullets, 0, bullets);

				bPicked = true;
				OnRep_Taken(); // server side
			}
			Deactivate();
		}
	}
}

void ASoulMP::SetDeactivateTimer(float TimeToLive)
{
	if ((HasAuthority() /*|| bTearOff*/) && !IsPendingKill())
	{
		if (TimeToLive > 0.0f){
			GetWorldTimerManager().SetTimer(TimerHandle_Deactivate, this, &ASoulMP::Deactivate, TimeToLive);
		}
		else{
			GetWorldTimerManager().ClearTimer(TimerHandle_Deactivate);
		}
	}
}

void ASoulMP::Deactivate_Implementation()
{
	OnDeactivate();

	GetWorldTimerManager().ClearTimer(TimerHandle_Deactivate);
	TimerHandle_Deactivate.Invalidate();

	if (GetCollisionComp() != nullptr)
		GetCollisionComp()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	if (GetSkeletalMeshComp() != nullptr)
		GetSkeletalMeshComp()->SetHiddenInGame(true);

	if (GetProjectileMovement() != nullptr)
	{
		GetProjectileMovement()->ProjectileGravityScale = 0.f;
		GetProjectileMovement()->BounceVelocityStopSimulatingThreshold = 5.f; // enable StopSimulating()
		GetProjectileMovement()->Velocity = FVector::ZeroVector;
	}

	SetLifeSpan(DeactivatedLifeSpan);

	for (auto Item : WeapItems){
		Item->SetHiddenInGame(true);
	}
	KillParticles();
}

void ASoulMP::OnRep_Taken()
{
	UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
}

void ASoulMP::OnRep_SpawnDir()
{
	GetProjectileMovement()->ProjectileGravityScale = 1.f;
	GetProjectileMovement()->Velocity = SpawnDir * DefaultProjectileSpeed;
}

/************************************************************
	Replication List
************************************************************/
void ASoulMP::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASoulMP, SpawnDir);
	DOREPLIFETIME(ASoulMP, bPicked);
}