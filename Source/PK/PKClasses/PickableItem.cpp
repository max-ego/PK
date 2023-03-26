// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "PickableItem.h"
#include "UnrealNetwork.h"
#include "Main/NetPlayer.h"
#include "Main/Util.h"


// Sets default values
APickableItem::APickableItem(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));

	NetUpdateFrequency = 30.f;
	bReplicates = true;
	bReplicateMovement = false;

	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	GetCollisionComp()->InitSphereRadius(120.0f);

	GetCollisionComp()->BodyInstance.SetCollisionProfileName("OverlapAll");

	GetCollisionComp()->OnComponentHit.AddDynamic(this, &APickableItem::OnHit); // set up a notification for when this component hits something blocking
	BeginOverlapDelegate.BindUFunction(this, FName("OnBeginOverlap"));
	GetCollisionComp()->OnComponentBeginOverlap.Add(BeginOverlapDelegate);

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComponent->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	StaticMeshComponent->bGenerateOverlapEvents = false;

	RootComponent = GetCollisionComp();

	GetSkeletalMeshComp()->Mobility = EComponentMobility::Movable;
	GetSkeletalMeshComp()->AttachParent = GetCollisionComp();

	GetStaticMeshComp()->Mobility = EComponentMobility::Movable;
	GetStaticMeshComp()->AttachParent = GetCollisionComp();
		
	/*AmmoAdd = 10;*/
	RespawnTime = 15;

	// note: applying AttenuationSettings here causes crash in PIE mode on 'QuitGame'
	AttenuationSettings = CreateDefaultSubobject<USoundAttenuation>(TEXT("PickItemSoundAttenuation"));
	AttenuationSettings->Attenuation.DistanceAlgorithm = ESoundDistanceModel::ATTENUATION_NaturalSound;
	AttenuationSettings->Attenuation.dBAttenuationAtMax = -45.f;
	AttenuationSettings->Attenuation.OmniRadius = 400.f;

	static ConstructorHelpers::FObjectFinder<USoundBase> RespawnSnd(TEXT("SoundWave'/Game/Sounds/specials/respawns/respawn_objects.respawn_objects'"));
	RespawnSound = RespawnSnd.Object;
	if (!(WITH_EDITOR || IsRunningDedicatedServer()))
	{
		RespawnSound->AttenuationSettings = AttenuationSettings;
		if (PickupSound) PickupSound->AttenuationSettings = AttenuationSettings;
	}
}

// Called when the game starts or when spawned
void APickableItem::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void APickableItem::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void APickableItem::OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
}

void APickableItem::OnBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool fromSweep, const FHitResult& Hit)
{
	if (HasAuthority() && OtherActor)
	{
		if (OtherActor->IsA(ANetPlayer::StaticClass()))
		{
			ANetPlayer* Pawn = Cast<ANetPlayer>(OtherActor);
			ANetPlayerController* PC = Cast<ANetPlayerController>(Pawn->Controller);
			if (PC)
			{
				if (OnTake(PC))
				{
					Deactivate();
					GetWorldTimerManager().SetTimer(TimerHandle, this, &APickableItem::Activate, RespawnTime);
				}
			}
		}
	}
}

bool APickableItem::OnTake(ANetPlayerController* PC)
{
	return PC->AmmoAdd(AmmoType, AmmoAdd, AltAmmoAdd);
}

void APickableItem::Respawn()
{

}

void APickableItem::OnRep_Toggle()
{	
	if (bFirstTime){ // run once when the client join
		if (bActive) HideItem(false);
		else HideItem(true);
	}
}

void APickableItem::HideItem(bool bHide)
{
	bFirstTime = false;
	if (HasAuthority()) bActive = !bHide;

	if (GetCollisionComp() != nullptr)
	{
		if (bHide)
			GetCollisionComp()->BodyInstance.SetCollisionProfileName("NoCollision");
		else
			GetCollisionComp()->BodyInstance.SetCollisionProfileName("OverlapAll");
	}

	if (GetStaticMeshComp() != nullptr)
		GetStaticMeshComp()->SetHiddenInGame(bHide);

	if (GetSkeletalMeshComp() != nullptr)
		GetSkeletalMeshComp()->SetHiddenInGame(bHide);
}

void APickableItem::Deactivate_Implementation()
{
	HideItem(true);

	if (PickupSound) UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
}

void APickableItem::Activate_Implementation()
{
	HideItem(false);

	if (RespawnSound) UGameplayStatics::PlaySoundAtLocation(this, RespawnSound, GetActorLocation());

	// check if we are overlapping on respawn
	FVector Loc = GetActorLocation();
	SetActorLocation(Loc + FVector(0, 0, 1), true);
	SetActorLocation(Loc);
}

#if WITH_EDITOR
bool APickableItem::CanEditChange(const UProperty* InProperty) const
{
	const bool ParentVal = Super::CanEditChange(InProperty);

	// Can we edit AmmoAdd?
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(APickableItem, AmmoAdd))
	{
		return (
			AmmoType == EAmmoType::stake ||
			AmmoType == EAmmoType::grenade ||
			AmmoType == EAmmoType::shell ||
			AmmoType == EAmmoType::StakeGunGL ||
			AmmoType == EAmmoType::Shotgun ||
			AmmoType == EAmmoType::MiniGunRL ||
			AmmoType == EAmmoType::health ||
			AmmoType == EAmmoType::megahealth
			);
	}

	// Can we edit AltAmmoAdd?
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(APickableItem, AltAmmoAdd))
	{
		return (
			AmmoType == EAmmoType::icebullet ||
			AmmoType == EAmmoType::bullets ||
			AmmoType == EAmmoType::StakeGunGL ||
			AmmoType == EAmmoType::Shotgun ||
			AmmoType == EAmmoType::MiniGunRL
			);
	}

	return ParentVal;
}
#endif

/************************************************************
	Replication List
************************************************************/
void APickableItem::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APickableItem, bActive);
}