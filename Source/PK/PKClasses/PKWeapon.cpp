// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "PKWeapon.h"
#include "Main/NetPlayer.h"
#include "Main/NetPlayerController.h"


// Sets default values
APKWeapon::APKWeapon(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{ 	
	PrimaryActorTick.bCanEverTick = true;
	//bCanBeDamaged = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = Scene;	
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	WeaponMesh->AttachParent = GetRootComponent();
	WeaponMesh->bCastDynamicShadow = false;
	WeaponMesh->CastShadow = false;
	//WeaponMesh->bSelfShadowOnly = true;
	WeaponMesh->SetRenderCustomDepth(true);

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComp"));
	AudioComponent->AttachParent = GetRootComponent();
	AudioComponent->bAutoActivate = false;

	OutOfAmmoAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("OutOfAmmoAudioComp"));
	OutOfAmmoAudioComponent->AttachParent = GetRootComponent();
	OutOfAmmoAudioComponent->bAutoActivate = false;

	ProjectileClass = APKProjectile::StaticClass();
	ComboProjectileClass = APKProjectile::StaticClass();

	static ConstructorHelpers::FObjectFinder<UObject>OutOfAmmoCue(TEXT("SoundCue'/Game/Sounds/weapons/assault-rifle/Cue/rifle_emptySoundCue.rifle_emptySoundCue'"));
	OutOf_Ammo = (USoundCue*)OutOfAmmoCue.Object;
	OutOfAmmo = UTIL.GetSound("weapons/assault-rifle", "weapon_rifle_empty");
}

// Called when the game starts or when spawned
void APKWeapon::BeginPlay()
{
	Super::BeginPlay();
	AActor* Parent = GetAttachParentActor();
	if (Parent){
		if (Parent->IsA(ANetPlayer::StaticClass()))
		{
			PLAYER = Cast<ANetPlayer>(Parent);
			SetOwner(PLAYER);
		}
	}
	WeaponMesh->SetOnlyOwnerSee(true);
	
	InitFireTimeOut();
}

// Called every frame
void APKWeapon::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

bool APKWeapon::CheckAmmo(TSubclassOf<class APKProjectile> ProjectileClass)
{
	ANetPlayerController* PC = Cast<ANetPlayerController>(PLAYER->Controller);
	uint8 AmmoType = ProjectileClass.GetDefaultObject()->AmmoType;

	if (AmmoType == 0x00 || AmmoType == 0x80) return true; // painhead painrotor
	
	if ((PLAYER->Role == ROLE_AutonomousProxy && (PC->LockedWeapons & AmmoType)) || PC->GetAmmo(AmmoType) == 0) return false;
	if (PLAYER->Role == ROLE_AutonomousProxy && !PC->IsMaxAmmo(AmmoType)) PC->LockedWeapons |= AmmoType;

	return true;
}

bool APKWeapon::SpawnProjectile(TSubclassOf<class APKProjectile> ProjectileCls)
{
	if (CheckAmmo(ProjectileCls))
	{
		PLAYER->SpawnProjectile(ProjectileCls);
		return true;
	}
	else
	{
		/*UTIL.PlaySnd(OutOfAmmoAudioComponent, OutOf_Ammo);*/
		UGameplayStatics::PlaySoundAttached(OutOfAmmo, GetOwner()->GetRootComponent());
		return false;
	}
}

void APKWeapon::InitFireTimeOut()
{
	switch (WeaponType){
	case EWeapons::PainKiller:
		FireTimeout = 0.f;
		AltFireTimeout = 0.f;
		break;
	case EWeapons::Shotgun:
		FireTimeout = 0.667f;
		AltFireTimeout = 1.5f;
		break;
	case EWeapons::StakeGunGL:
		FireTimeout = 1.2f;
		AltFireTimeout = 0.6f;
		break;
	case EWeapons::MiniGunRL:
		FireTimeout = 0.8f;
		AltFireTimeout = 0.f;
		break;
	case EWeapons::DriverElectro:
		FireTimeout = 0.2f;
		AltFireTimeout = 0.f;
		break;
	case EWeapons::RifleFlameThrower:
		FireTimeout = 0.07f;
		AltFireTimeout = 0.1f;
		break;
	case EWeapons::BoltGunHeater:
		FireTimeout = 1.33f;
		AltFireTimeout = 1.f;
		break;
	case EWeapons::DemonGun:
		FireTimeout = 0.5f;
		AltFireTimeout = 0.f;
		break;
	default:
		FireTimeout = 0.f;
		AltFireTimeout = 0.f;
		break;
	}
}

bool APKWeapon::CheckFireTimeOut()
{
	if (bAltFirePressed &&
		WeaponType != EWeapons::Shotgun)
		return false;

	if (GetWorldTimerManager().IsTimerActive(FireTimerHandle))
	{
		bFirePressed = true;
		return false;
	}
	UpdateFireTimeOut(EFireType::Fire);
	return true;
}

bool APKWeapon::CheckAltFireTimeOut()
{
	if (bFirePressed) return false;

	if (GetWorldTimerManager().IsTimerActive(AltFireTimerHandle))
	{
		bAltFirePressed = true;
		return false;
	}
	UpdateFireTimeOut(EFireType::AltFire);
	return true;
}

void APKWeapon::UpdateFireTimeOut(TEnumAsByte<EFireType> FireType)
{
	if (FireType == EFireType::Fire)
	{
		GetWorldTimerManager().SetTimer(FireTimerHandle, this, &APKWeapon::OnFireTimeOut, FireTimeout, false);
		if (AltFireTimeout > 0 && FireTimeout > 0)
		{
			float TimeOut = FMath::Min<float>(GetWorldTimerManager().GetTimerRemaining(AltFireTimerHandle), FireTimeout);
			GetWorldTimerManager().SetTimer(AltFireTimerHandle, this, &APKWeapon::OnAltFireTimeOut, TimeOut, false);
		}
	}

	if (FireType == EFireType::AltFire)
	{
		GetWorldTimerManager().SetTimer(AltFireTimerHandle, this, &APKWeapon::OnAltFireTimeOut, AltFireTimeout, false);
		if (FireTimeout > 0 && AltFireTimeout > 0)
		{
			float TimeOut = FMath::Min<float>(GetWorldTimerManager().GetTimerRemaining(FireTimerHandle), AltFireTimeout);
			GetWorldTimerManager().SetTimer(FireTimerHandle, this, &APKWeapon::OnFireTimeOut, TimeOut, false);
		}
	}
}

bool APKWeapon::Fire()
{
	if (CheckFireTimeOut())
	{
		bFirePressed = true;
		return true;
	}
	return false;
}

bool APKWeapon::OnFinishFire()
{
	OutOfAmmoAudioComponent->Stop();

	if (bFirePressed)
	{
		bFirePressed = false;
		return true;
	}	
	return false;
}

bool APKWeapon::AltFire()
{
	if (CheckAltFireTimeOut())
	{
		bAltFirePressed = true;
		return true;
	}
	return false;
}

bool APKWeapon::OnFinishAltFire()
{
	OutOfAmmoAudioComponent->Stop();

	if (bAltFirePressed)
	{
		bAltFirePressed = false; 
		return true;
	}	
	return false;
}

void APKWeapon::OnHeadBack(){};

void APKWeapon::OnOwnerUnPossessed()
{
	AudioComponent->Stop();
	OutOfAmmoAudioComponent->Stop();
}

void APKWeapon::PLAY(UAnimMontage* montage)
{
	if (montage != NULL && WeaponMesh != NULL)
	{
		UAnimInstance* AnimInstance = WeaponMesh->GetAnimInstance();
		if (AnimInstance != NULL){
			AnimInstance->Montage_Play(montage, 1.f);
		}
	}
}

void APKWeapon::PLAYSND(USoundCue* SoundCue)
{
	AudioComponent->Stop();
	if (SoundCue != NULL){
		AudioComponent->SetSound(SoundCue);
		AudioComponent->Play();
	}
}