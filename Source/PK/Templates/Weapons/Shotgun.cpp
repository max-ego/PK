// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "Shotgun.h"
#include "Main/NetPlayer.h"

AShotgun::AShotgun(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	SGAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("SGAudioComp"));
	SGAudioComponent->AttachParent = GetRootComponent();
	SGAudioComponent->bAutoActivate = false;

	static ConstructorHelpers::FObjectFinder<UClass>Projectile(TEXT("Class'/Script/PK.SGShot'"));
	ProjectileClass = (UClass*)Projectile.Object;

	static ConstructorHelpers::FObjectFinder<UObject>shot(TEXT("AnimMontage'/Game/Models/Weapons/ASG/shot.shot'"));
	fire = (UAnimMontage*)shot.Object;

	static ConstructorHelpers::FObjectFinder<UObject>iceshot(TEXT("AnimMontage'/Game/Models/Weapons/ASG/iceshot.iceshot'"));
	altfire = (UAnimMontage*)iceshot.Object;

	static ConstructorHelpers::FObjectFinder<UObject>shotcue(TEXT("SoundCue'/Game/Sounds/weapons/shotgun/Cue/Shot.Shot'"));
	Shotgun = (USoundCue*)shotcue.Object;

	static ConstructorHelpers::FObjectFinder<UObject>iceshotcue(TEXT("SoundCue'/Game/Sounds/weapons/shotgun/Cue/IceShot.IceShot'"));
	Freezer = (USoundCue*)iceshotcue.Object;

	WeaponType = EWeapons::Shotgun;
}

void AShotgun::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AShotgun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AShotgun::Fire()
{	
	if (!Super::Fire()) return false;

	if (SpawnProjectile(ProjectileClass))
	{
		UTIL.PlaySnd(SGAudioComponent, Shotgun);
		PLAY(fire);

		return true;
	}
	return false;
}

bool AShotgun::OnFinishFire()
{
	if (!Super::OnFinishFire()) return false;

	return true;
}

bool AShotgun::AltFire()
{
	if (!Super::AltFire()) return false;

	if (SpawnProjectile(AltProjectileClass))
	{
		UTIL.PlaySnd(AudioComponent, Freezer);
		PLAY(altfire);

		return true;
	}
	return false;
}

bool AShotgun::OnFinishAltFire()
{
	if (!Super::OnFinishAltFire()) return false;

	return true;
}


