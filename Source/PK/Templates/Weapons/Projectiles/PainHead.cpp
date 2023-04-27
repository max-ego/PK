// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "PainHead.h"
#include "Main/Util.h"
#include "UnrealNetwork.h"

APainHead::APainHead(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicateMovement = false;

	GetCollisionComp()->InitSphereRadius(12.5f);
		
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>ProjectileMesh(TEXT("SkeletalMesh'/Game/Models/PKW_blades/PKW_blades.PKW_blades'"));
	GetSkeletalMeshComp()->SetSkeletalMesh(ProjectileMesh.Object);
	GetSkeletalMeshComp()->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	GetSkeletalMeshComp()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	
	DefaultProjectileSpeed = PainHeadSpeed;
	GetProjectileMovement()->InitialSpeed = DefaultProjectileSpeed;
	GetProjectileMovement()->MaxSpeed = DefaultProjectileSpeed;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> Particle(TEXT("ParticleSystem'/Game/Particles/PainHeadBeam.PainHeadBeam'"));
	HeadBeamComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("HeadBeamComp"));
	HeadBeamComp->SetTemplate(Particle.Object);
	HeadBeamComp->bAutoActivate = false;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> HitParticle(TEXT("/Game/Particles/shotgunhitwall.shotgunhitwall"));
	HitEnemyParticle = HitParticle.Object;
	
	electro_loop = UTIL.GetSound("weapons/painkiller", "painkiller-electro-chime-loop");

	ElectroLoopAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("ElectroLoopAudioComp"));
	ElectroLoopAudioComp->AttachParent = HeadBeamComp;
	ElectroLoopAudioComp->bAutoActivate = false;

	PainkillerStick = UTIL.GetSound("impacts", "painkiller-default2");
	
	static ConstructorHelpers::FObjectFinder<USoundCue>ElectroShockLoopSndCue(TEXT("SoundCue'/Game/Sounds/weapons/painkiller/Cue/ElectroShockLoop.ElectroShockLoop'"));
	ElectroShockLoop = ElectroShockLoopSndCue.Object;
	static ConstructorHelpers::FObjectFinder<USoundCue>ElectroLoopSndCue(TEXT("SoundCue'/Game/Sounds/weapons/painkiller/Cue/ElectroLoop.ElectroLoop'"));
	ElectroLoop = ElectroLoopSndCue.Object;
	ElectroLoopAudioComp->SetSound(ElectroLoop);

	PainkillerShoot = UTIL.GetSound("weapons/painkiller", "painkiller-shoot");
	static ConstructorHelpers::FObjectFinder<UObject>PHBack(TEXT("SoundCue'/Game/Sounds/weapons/painkiller/Cue/PHBack.PHBack'"));
	PainHeadBack = (USoundCue*)PHBack.Object;
	DeactivatedLifeSpan = 1.0f;

	bPainHead = true;
	DamageType = UPainheadDamage::StaticClass();
	SpecificDamageType = UPainbeamDamage::StaticClass();

	Damage = PainHeadDamage;
}

// Called when the game starts or when spawned
void APainHead::BeginPlay()
{
	Super::BeginPlay();

	Owner = Cast<ANetPlayer>(GetOwner());

	if (Owner && Owner->GetCurrWeapon()){

		USkeletalMeshComponent* Mesh = NULL;
		FName AttachPointName = NAME_None;

		if (b3rdPerson)
		{
			Mesh = Owner->GetMesh();
			AttachPointName = TEXT("pkheadSocket");
		}
		else{
			Mesh = Cast<APKWeapon>(Owner->GetCurrWeapon())->GetWeaponMesh();
			AttachPointName = TEXT("joint5Socket");
		}
		
		FParticleSysParam Target;
		Target.Name = TEXT("Target");
		Target.ParamType = EParticleSysParamType::PSPT_Actor;
		Target.Actor = this;
		HeadBeamComp->InstanceParameters.Add(Target);

		if (Mesh) {
			HeadBeamComp->AttachTo(Mesh, AttachPointName, EAttachLocation::SnapToTarget, true);
			HeadBeamComp->Activate();
		}
		
		if (b3rdPerson)
		{
			OnRep_BeamToggle();
			UGameplayStatics::PlaySoundAttached(PainkillerShoot, Owner->GetRootComponent());
		}
	}
}

void APainHead::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (bHideBeam)
		ElectroLoopAudioComp->Stop();
	else
		ElectroLoopAudioComp->Play();
	
	if (bShock)
		ElectroLoopAudioComp->SetSound(ElectroShockLoop);
	else
		ElectroLoopAudioComp->SetSound(ElectroLoop);
}

// Called every frame
void APainHead::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!bHitScanEnabled) return;
	BeamIntersection(DeltaTime);
	ApplySpecificDamage(PainRayDamage, DeltaTime);
}

void APainHead::OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::OnHit(OtherActor, OtherComp, NormalImpulse, Hit);

	UGameplayStatics::PlaySoundAtLocation(this, PainkillerStick, GetActorLocation());
}

void APainHead::OnBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool fromSweep, const FHitResult& Hit)
{
	Super::OnBeginOverlap(OtherActor, OtherComp, OtherBodyIndex, fromSweep, Hit);

}

void APainHead::HookUpEnemy(AActor* OtherActor)
{
	ANetPlayer* Enemy = Cast<ANetPlayer>(OtherActor);
	UPKCharacterMovementComponent* CharacterMovement = Cast<UPKCharacterMovementComponent>(Enemy->GetCharacterMovement());
	FVector DragImpulseDir = 0.075f * (GetOwner()->GetActorLocation() - Enemy->GetActorLocation()).GetSafeNormal2D() + FVector(0, 0, 1);
	
	float ImpulseMagnitude = CharacterMovement->JumpZVelocity * 1.5f;
	if (CharacterMovement->IsFalling()) ImpulseMagnitude *= 1.75f;
	
	CharacterMovement->ReceiveImpactRPC(DragImpulseDir * ImpulseMagnitude);
}

void APainHead::BeamIntersection(float DeltaTime)
{
	if (!(HeadBeamComp != nullptr &&
		HeadBeamComp->IsValidLowLevel() &&
		!HeadBeamComp->IsPendingKill())
		) return;

	if (!(Owner && Owner->IsValidLowLevel() && !Owner->IsPendingKill()))
	{
		if (HasAuthority() && IsValidLowLevel() && !IsPendingKill()) Destroy();
		return;
	}

	FVector HitLine = (GetActorLocation() - HeadBeamComp->GetComponentLocation()).GetSafeNormal();
	FVector SightLine = Owner->GetBaseAimRotation().RotateVector(FVector::ForwardVector);
	float d = FMath::PointDistToLine(GetActorLocation(), SightLine, HeadBeamComp->GetComponentLocation());

	if ((HitLine | SightLine) < 0 || d > 2 * 84){
		ApplyRemainingDamage();
		BeamShockSnd(false);
		HideBeam(true);
		return;
	}

	FHitResult Hit = FHitResult(1.f);
	TArray<FHitResult, FDefaultAllocator> OutHits;
	const FVector End = GetActorLocation();

	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic);
	ObjectParams.AddObjectTypesToQuery(ECollisionChannel::ECC_PhysicsBody);

	static const FName LineTraceMultiName(TEXT("LineTraceMulti"));
	FCollisionQueryParams Params = FCollisionQueryParams(LineTraceMultiName, true, Owner);
	//Params.bReturnPhysicalMaterial = true;
	//Params.bTraceAsyncScene = true;
	Params.AddIgnoredActor(this);
	bool bHit = GetWorld()->LineTraceMulti(OutHits, HeadBeamComp->GetComponentLocation(), End, Params, ObjectParams);	
	
	bool bShowBeam = true;
	if (bHit)
	{
		for (auto OutHit : OutHits)
		{
			if (OutHit.GetActor()->IsA(AStaticMeshActor::StaticClass()))
			{
				bShowBeam = false; break;
			}
			else if (bHit &&
				(OutHit.GetComponent()->IsA(UStaticMeshComponent::StaticClass())
				|| OutHit.GetComponent()->IsA(USkeletalMeshComponent::StaticClass())
				&& !OutHit.GetComponent()->IsAnySimulatingPhysics())
				)
			{
				Hit = OutHit; bHit = false; // take the first hit
			}
		}
		if (bShowBeam)
		{
			if (Hit.bBlockingHit)
			{
				if (Enemy != Hit.GetActor())
				{
					ApplyRemainingDamage();
					Enemy = Hit.GetActor();
					if (HasAuthority() && PC && Hit.GetActor()->IsA(ANetPlayer::StaticClass()))
						PC->bSpecificHitSound = true;
					bHitEnemy = true;
				}
			}
			else ApplyRemainingDamage();
		}
		else ApplyRemainingDamage();
	}
	else ApplyRemainingDamage();
	
	BeamShockSnd(bHitEnemy);
	HideBeam(!bShowBeam);

	if (ParticleTimeout <= 0)
	{
		if (bHitEnemy)
		{
			// TODO: hit enemy particle and sound
			UGameplayStatics::SpawnEmitterAttached(
				HitEnemyParticle,
				GetRootComponent(),
				NAME_None,
				Hit.ImpactPoint,
				Hit.ImpactNormal.Rotation(),
				EAttachLocation::KeepWorldPosition,
				true
				);
		}
		else
		{

		}
		ParticleTimeout = 0.10f;
	}
}

void APainHead::Deactivate_Implementation()
{
	Super::Deactivate_Implementation();

	if (b3rdPerson) UTIL.PlaySnd(AudioComponent, PainHeadBack);
	
	DestroyBeam();
}

void APainHead::OnRep_PainHeadWithdraw()
{
	Super::OnRep_PainHeadWithdraw();

	DestroyBeam();
}

void APainHead::HideBeam(bool Hide)
{
	if (!HasAuthority()) return;

	if (bHideBeam != Hide){
		bHideBeam = Hide;
		OnRep_BeamToggle(); // server
	}
}

void APainHead::OnRep_BeamToggle()
{
	if (HeadBeamComp != nullptr
		&& HeadBeamComp->IsValidLowLevel()
		&& !HeadBeamComp->IsPendingKill())
	{
		HeadBeamComp->SetHiddenInGame(bHideBeam);

		if (bHideBeam)
			ElectroLoopAudioComp->Stop();
		else
			ElectroLoopAudioComp->Play();
	}
}

void APainHead::BeamShockSnd(bool bShockSnd)
{
	if (!HasAuthority()) return;

	if (bShock != bShockSnd){
		bShock = bShockSnd;
		OnRep_BeamToggleSnd(); // server
	}
}

void APainHead::OnRep_BeamToggleSnd()
{
	if (HeadBeamComp != nullptr
		&& HeadBeamComp->IsValidLowLevel()
		&& !HeadBeamComp->IsPendingKill())
	{
		if (bShock)
			ElectroLoopAudioComp->SetSound(ElectroShockLoop);
		else
			ElectroLoopAudioComp->SetSound(ElectroLoop);
	}
}

void APainHead::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APainHead, bHideBeam);
	DOREPLIFETIME(APainHead, bShock);
}