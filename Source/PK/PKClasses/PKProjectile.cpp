// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "PKProjectile.h"
#include "UnrealNetwork.h"
#include "Main/NetPlayer.h"
#include "Main/Util.h"
#include "PKClasses/PickableItem.h"


// Sets default values
APKProjectile::APKProjectile(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	
	NetUpdateFrequency = 30.f;
	bReplicates = true;
	bReplicateMovement = false; // for rectilinearly moving projectiles should be 'false'

 	PrimaryActorTick.bCanEverTick = true;

	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	GetCollisionComp()->BodyInstance.SetCollisionProfileName("OverlapAll");
	GetCollisionComp()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

	GetCollisionComp()->OnComponentHit.AddDynamic(this, &APKProjectile::OnHit); // set up a notification for when this component hits something blocking
	BeginOverlapDelegate.BindUFunction(this, FName("OnBeginOverlap"));
	GetCollisionComp()->OnComponentBeginOverlap.Add(BeginOverlapDelegate);

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	GetStaticMeshComp()->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	GetStaticMeshComp()->bGenerateOverlapEvents = false;
	
	RootComponent = GetCollisionComp();

	GetSkeletalMeshComp()->Mobility = EComponentMobility::Movable;
	GetSkeletalMeshComp()->AttachParent = GetCollisionComp();
	
	GetStaticMeshComp()->Mobility = EComponentMobility::Movable;
	GetStaticMeshComp()->AttachParent = GetCollisionComp();
	
	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	GetProjectileMovement()->UpdatedComponent = GetCollisionComp();
	GetProjectileMovement()->ProjectileGravityScale = 0.f;

	GetProjectileMovement()->InitialSpeed = DefaultProjectileSpeed;
	GetProjectileMovement()->MaxSpeed = DefaultProjectileSpeed;
	
	/*avoid PainHead 'sticking' after blocking hit*/
	GetProjectileMovement()->bShouldBounce = true;
	GetProjectileMovement()->Bounciness = 0.f;
	GetProjectileMovement()->BounceVelocityStopSimulatingThreshold = 0.f; // disable StopSimulating()

	OnProjectileDestoyedDelegate.BindUFunction(this, FName("PainHeadReload"));

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComp"));
	AudioComponent->AttachParent = GetRootComponent();
	AudioComponent->bAutoActivate = false;

	DamageType = UPKDamageType::StaticClass();
	SpecificDamageType = UPKDamageType::StaticClass();
}

void APKProjectile::GatherCurrentMovement()
{
	Super::GatherCurrentMovement();

	ReplicatedMovement.LinearVelocity = GetProjectileMovement()->Velocity;
}

void APKProjectile::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();

	GetProjectileMovement()->Velocity = ReplicatedMovement.LinearVelocity;
}

void APKProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
}

// Called when the game starts or when spawned
void APKProjectile::BeginPlay()
{
	Super::BeginPlay();

	ACharacter* Pawn = Cast<ACharacter>(Instigator);
	if (Pawn) PC = Cast<ANetPlayerController>(Pawn->Controller);

	SetDeactivateTimer(TimeToLive);
	
	APawn* player = NULL;

	if (!IsRunningDedicatedServer())
	{
		UPKGameInstance* GI = Cast<UPKGameInstance>(GetWorld()->GetGameInstance());
		ULocalPlayer* const LocalPlayer = GI->GetFirstGamePlayer();
		player = LocalPlayer->PlayerController->AcknowledgedPawn;
	}
	
	b3rdPerson = !(player && player == GetOwner());
}

// Called every frame
void APKProjectile::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (ParticleTimeout > 0) ParticleTimeout -= DeltaTime;
	
	if (bPainHead && bPainHeadWithdraw && GetOwner())
	{
		FVector Dest = GetOwner()->GetActorLocation();
		FVector Dir = Dest - GetActorLocation();

		GetProjectileMovement()->Velocity = Dir.GetSafeNormal() * PainHeadBackSpeed;

		if (HasAuthority())
		{
			if (Dir.Size() <= 126.f)
			{
				Deactivate();
				PainHeadReload();
			}
		}
	}
}

void APKProjectile::OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	HitResult = Hit;

	GetWorldTimerManager().ClearTimer(TimerHandle_GetPainHeadBack);
	
	// force stop projectile since StopSimulating() is disabled
	GetProjectileMovement()->Velocity = FVector::ZeroVector;
	if (AmmoType == 0x00){
		GetCollisionComp()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	}

	if (OtherActor && OtherActor != GetOwner())
	{
		if (IsValidLowLevel() && !IsPendingKill())
		{
			if (HasAuthority())
			{
				switch (AmmoType){
				case 0x00:
					if (bCombo ||
						OtherActor->IsA(APKProjectile::StaticClass()) ||
						OtherActor->IsA(ADestructibleItemBase::StaticClass())
						) ActivateWithdraw();
					if (OtherActor->IsA(ADestructibleItemBase::StaticClass())) ApplyDamage(OtherActor);
					break;
				default:
					Deactivate();
				}
			}
		}
	}
}

void APKProjectile::OnBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool fromSweep, const FHitResult& Hit)
{
	if (OtherActor &&
		(OtherActor->IsA(APickableItem::StaticClass()) ||
		!(OtherActor->IsA(ACharacter::StaticClass())/* || OtherActor->IsA(ADestructibleItemBase::StaticClass())*/)
		)
		)
	{
		return;
	}

	HitResult = Hit;

	if (OtherActor && GetOwner() && (OtherActor != GetOwner() || bHitOwner))
	{
		if (IsValidLowLevel() && !IsPendingKill()) {
			if (HasAuthority())	{
				switch (AmmoType){
					case 0x00: // painhead
						if (!bPainHeadWithdraw) // affect once
						{
							ApplyDamage(OtherActor);
							HookUpEnemy(OtherActor);
						}
						ActivateWithdraw();
						break;
					case 0x02: // rocket & grenade
						Deactivate();
						break;
					default:
						Deactivate();
						ApplyDamage(OtherActor);
				}
			}
		}
	}
}

void APKProjectile::SetDeactivateTimer(float TimeToLive)
{
	if ((HasAuthority()) && !IsPendingKill())
	{
		// Reload PainHead if destroyed by the KillZ
		if (bPainHead) OnDestroyed.AddUnique(OnProjectileDestoyedDelegate);

		if (TimeToLive > 0.0f){
			GetWorldTimerManager().ClearTimer(TimerHandle_GetPainHeadBack);

			GetWorldTimerManager().SetTimer(TimerHandle_Deactivate, this, &APKProjectile::Deactivate, TimeToLive);
		}
		else{
			GetWorldTimerManager().ClearTimer(TimerHandle_Deactivate);

			float GoBackTime = PainHeadRange / DefaultProjectileSpeed;
			GetWorldTimerManager().SetTimer(TimerHandle_GetPainHeadBack, this, &APKProjectile::ActivateWithdraw, GoBackTime);
		}
	}
}

void APKProjectile::Deactivate_Implementation()
{
	if (!bIsActive) return;
	bIsActive = false;

	OnDeactivate();

	GetWorldTimerManager().ClearTimer(TimerHandle_Deactivate);
	TimerHandle_Deactivate.Invalidate();

	if (GetCollisionComp() != nullptr)
		GetCollisionComp()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	
	if (GetStaticMeshComp() != nullptr)
		GetStaticMeshComp()->SetHiddenInGame(true);

	if (GetSkeletalMeshComp() != nullptr)
		GetSkeletalMeshComp()->SetHiddenInGame(true);

	if (GetProjectileMovement() != nullptr)
	{
		GetProjectileMovement()->ProjectileGravityScale = 0.f;
		GetProjectileMovement()->BounceVelocityStopSimulatingThreshold = 5.f; // enable StopSimulating()
		GetProjectileMovement()->Velocity = FVector::ZeroVector;
	}
	
	SetActorTickEnabled(false); // still got at least one tick after
	bHitScanEnabled = false; // ensure no more HitScan() occur
	ApplyRemainingDamage();

	SetLifeSpan(DeactivatedLifeSpan);
}

void APKProjectile::PainHeadReload()
{
	if (bPainHead){
		bPainHead = false;
		if (GetNetOwningPlayer())
		{
			ANetPlayer* Owner = Cast<ANetPlayer>(GetOwner());
			Owner->OnReturnPainHead();
			Owner->HideThirdPersonPainHead(false);
		}
	}
}

void APKProjectile::ApplyDamage(AActor* OtherActor, TSubclassOf<UPKDamageType> DamageTypeClass)
{
	if (DamageTypeClass != NULL) DamageType = DamageTypeClass;
	if (OtherActor && PC) UGameplayStatics::ApplyDamage(OtherActor, Damage, PC, this, DamageType);
}

void APKProjectile::OnRep_PainHeadWithdraw()
{
	GetCollisionComp()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
}

void APKProjectile::OnRep_AddVelocity()
{
	GetProjectileMovement()->ProjectileGravityScale = 1.f;
	GetProjectileMovement()->Velocity = AddVelocity;
}

void APKProjectile::OnRep_ProjectileSpeed()
{
	GetProjectileMovement()->InitialSpeed = ProjectileSpeed;
	GetProjectileMovement()->MaxSpeed = ProjectileSpeed;
}

void APKProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APKProjectile, bPainHeadWithdraw);
	DOREPLIFETIME(APKProjectile, AddVelocity);
	DOREPLIFETIME(APKProjectile, bHitEnemy);
	DOREPLIFETIME(APKProjectile, ProjectileSpeed);
}