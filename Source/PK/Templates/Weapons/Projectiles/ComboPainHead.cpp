// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "ComboPainHead.h"
#include "Main/Util.h"

AComboPainHead::AComboPainHead(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	GetCollisionComp()->InitSphereRadius(12.5f);
	
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>ProjectileMesh(TEXT("SkeletalMesh'/Game/Models/PKW_blades/PKW_blades.PKW_blades'"));
	GetSkeletalMeshComp()->SetSkeletalMesh(ProjectileMesh.Object);
	GetSkeletalMeshComp()->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	GetSkeletalMeshComp()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	DefaultProjectileSpeed = PainHeadSpinningSpeed;
	GetProjectileMovement()->InitialSpeed = DefaultProjectileSpeed;
	GetProjectileMovement()->MaxSpeed = DefaultProjectileSpeed;

	static ConstructorHelpers::FObjectFinder<USoundCue>RotorSndCue(TEXT("SoundCue'/Game/Sounds/weapons/painkiller/Cue/RotorLoop.RotorLoop'"));
	RotorLoop = RotorSndCue.Object;

	PainkillerShoot = UTIL.GetSound("weapons/painkiller", "painkiller-shoot");
	static ConstructorHelpers::FObjectFinder<UObject>PHBack(TEXT("SoundCue'/Game/Sounds/weapons/painkiller/Cue/PHBack.PHBack'"));
	PainHeadBack = (USoundCue*)PHBack.Object;
	DeactivatedLifeSpan = 1.0f;
	
	bPainHead = true; 
	bCombo = true;
	Damage = 1000.f;
	DamageType = UPainrotorDamage::StaticClass();
	
	TimeToLive = 0.f; // never die
}

// Called when the game starts or when spawned
void AComboPainHead::BeginPlay()
{
	Super::BeginPlay();

	UTIL.PlaySnd(AudioComponent, RotorLoop);
}

// Called every frame
void AComboPainHead::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void AComboPainHead::OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::OnHit(OtherActor, OtherComp, NormalImpulse, Hit);
}

void AComboPainHead::OnBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool fromSweep, const FHitResult& Hit)
{
	Super::OnBeginOverlap(OtherActor, OtherComp, OtherBodyIndex, fromSweep, Hit);
	
}

void AComboPainHead::Deactivate_Implementation()
{
	Super::Deactivate_Implementation();
	
	AudioComponent->Stop();

	if (b3rdPerson) UTIL.PlaySnd(AudioComponent, PainHeadBack);
}

