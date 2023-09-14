// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "BarrelBig_MP.h"
#include "UnrealNetwork.h"
#include "Main/Util.h"
#include "Main/NetPlayer.h"
#include "PKClasses/DamageTypes/ExplosionDamage.h"
#include "Templates/Weapons/Projectiles/Rocket.h"


// Sets default values
ABarrelBig_MP::ABarrelBig_MP(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> Particle(TEXT("/Game/Particles/Explosion.Explosion"));
	ExplosionParticle = Particle.Object;

	static ConstructorHelpers::FObjectFinder<USoundBase> ExplosionSound(TEXT("SoundWave'/Game/Sounds/impacts/barrel-wood-explode.barrel-wood-explode'"));
	ExplosionSnd = ExplosionSound.Object;
	/*ExplosionSnd = UTIL.GetSound("impacts", "barrel-wood-explode");*/

	static ConstructorHelpers::FObjectFinder<USoundBase> RespawnSnd(TEXT("SoundWave'/Game/Sounds/specials/respawns/respawn_objects.respawn_objects'"));
	RespawnSound = RespawnSnd.Object;
	/*RespawnSound = UTIL.GetSound("specials/respawns", "respawn_objects");*/

	static ConstructorHelpers::FObjectFinder<UStaticMesh>mesh(TEXT("StaticMesh'/Game/Items/BarrelBig.BarrelBig'"));
	GetStaticMeshComp()->SetStaticMesh(mesh.Object);
	GetStaticMeshComp()->bGenerateOverlapEvents = false;
	
	GetCollisionComp()->BodyInstance.SetCollisionProfileName("OverlapAll");

	RespawnTime = 10;
	DeactivateTimerDelegate.BindUFunction(this, FName("Deactivate"));

	BlendTime = FMath::FRandRange(0.5f, 2.0f);
}

// Called when the game starts or when spawned
void ABarrelBig_MP::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABarrelBig_MP::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	
	if (HasAuthority()/*NetMode == NM_Standalone || NetMode == NM_ListenServer || NetMode == NM_DedicatedServer*/)
	{
		if (LastHealth > RemainingHealth)
		{
			BlendValue += DeltaTime / BlendTime;
			BlendValue = FMath::Clamp(BlendValue, 0.f, 1.f);
			CurrHealth = LastHealth - (LastHealth - RemainingHealth) * BlendValue;
		}
		else{
			BlendValue = 0.f;
			LastHealth = CurrHealth;
		}

		if (CurrHealth <= 0.f)
		{
			bActive = false;
			GetWorldTimerManager().SetTimerForNextTick(DeactivateTimerDelegate);
		}
	}
}

void ABarrelBig_MP::TakeAnyDamage(float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (bActive)
	{
		RemainingHealth -= Damage;
		Instigator = InstigatedBy;
	}
}

void ABarrelBig_MP::Deactivate()
{
	RemainingHealth = Health;
	CurrHealth = Health;
	LastHealth = Health;
	BlendTime = FMath::FRandRange(0.5f, 2.0f);
	
	OnRep_Toggle(); // server side
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ABarrelBig_MP::Activate, RespawnTime);
}

void ABarrelBig_MP::Activate()
{
	CheckIfAnyPawnIsInsideMe();
	
	bActive = true;	
	OnRep_Toggle(); // server side
}

void ABarrelBig_MP::CheckIfAnyPawnIsInsideMe()
{
	/**
	* In some cases a pawn could get stuck between a respawned barrel and other geometry. 
	* So just schedule an explosion if we are overlapping any pawn on respawn.
	*/
	TArray<AActor*> Actors;
	GetOverlappingActors(Actors);
	float dist = 9999.f;
	for (auto actor : Actors)
	{
		bool bExplode = false;
		if (actor->IsA(ACharacter::StaticClass()))
		{
			bExplode = true;
			float d = GetDistanceTo(actor);
			if (d < dist)
			{
				// consider the nearest pawn an instigator
				dist = d;
				Instigator = Cast<ACharacter>(actor)->Controller;
			}
		}

		if (bExplode) GetWorldTimerManager().SetTimer(TimerHandle, this, &ABarrelBig_MP::Explode, 1.5f);
	}
}

void ABarrelBig_MP::OnRep_Toggle()
{
	if (bActive) // Respawn
	{
		if (GetStaticMeshComp() != nullptr)
		{
			GetStaticMeshComp()->SetHiddenInGame(0);
			GetStaticMeshComp()->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
		}
		UGameplayStatics::PlaySoundAtLocation(this, RespawnSound, GetActorLocation());
	}
	else // Explode
	{
		TArray<AActor*> Childs;
		GetAttachedActors(Childs);
		for (auto actor : Childs)
		{
			actor->Destroy();
		}
		
		if (GetStaticMeshComp() != nullptr)
		{
			GetStaticMeshComp()->SetHiddenInGame(1);
			GetStaticMeshComp()->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
		}

		if (DestroyPackClass)
		{
			UWorld* const World = GetWorld();
			if (World != NULL)
			{
				World->SpawnActor<ADestructibleActor>(DestroyPackClass, GetActorLocation(), GetActorRotation(), FActorSpawnParameters());
			}
		}

		FRotator SpawnRotation = GetActorRotation().RotateVector(FVector(0, 0, 1)).Rotation();
		FVector SpawnLocation = GetActorLocation() + GetActorRotation().RotateVector(FVector(0.f, 0.0f, -70.0f));

		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSnd, GetActorLocation());

		UGameplayStatics::SpawnEmitterAttached(
			ExplosionParticle,
			GetRootComponent(),
			NAME_None,
			SpawnLocation,
			SpawnRotation,
			EAttachLocation::KeepWorldPosition,
			true
			);
		
		// Damage
		TArray<ACharacter*> Pawns;
		UTIL.FindAllObjectsClassOf(Pawns);
		for (auto pawn : Pawns)
		{
			if (pawn->PlayerState)
			{
				TraceApplyDmg(pawn, this);
			}
		}

		TArray<ADestructibleItemBase*> Destructibles;
		UTIL.FindAllObjectsClassOf(Destructibles);
		for (auto actor : Destructibles)
		{
			TraceApplyDmg(actor, actor);
		}
		
		if (!IsRunningDedicatedServer())
		{
			UPKGameInstance* GI = Cast<UPKGameInstance>(GetWorld()->GetGameInstance());
			ANetPlayer* player = Cast<ANetPlayer>(GI->GetFirstLocalPlayerController()->AcknowledgedPawn);
			
			if (player)
			{
				/*check for obstacles between*/
				FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("Trace")), true, this);
				bool bBlockingHit = GetWorld()->LineTraceTestByProfile(GetActorLocation(), player->GetActorLocation(), TEXT("Spectator"), TraceParams);
		
				if (!bBlockingHit)
				{
					UPKCharacterMovementComponent* CharacterMovement = Cast<UPKCharacterMovementComponent>(player->GetCharacterMovement());
					CharacterMovement->ReceiveImpact(ARocket::GetExplosionVelocity(GetActorLocation(), CharacterMovement));
				}
			}
		}
	}
}

void ABarrelBig_MP::TraceApplyDmg(AActor* actor, AActor* IgnoreActor)
{
	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("Trace")), false, IgnoreActor);

	bool bBlockingHit = GetWorld()->LineTraceTestByProfile(GetActorLocation(), actor->GetActorLocation(), TEXT("Spectator"), TraceParams);
	float dist = (GetActorLocation() - actor->GetActorLocation()).Size();

	float ExplosionRange = 500.f;
	if (bBlockingHit || dist > ExplosionRange) return;

	float ImpactRatio = 1.f - FMath::Pow(dist / ExplosionRange, 2);
	UGameplayStatics::ApplyDamage(actor, 300.f * ImpactRatio, Instigator, this, UExplosionDamage::StaticClass());
}

/************************************************************
Replication List
************************************************************/
void ABarrelBig_MP::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABarrelBig_MP, bActive);
}