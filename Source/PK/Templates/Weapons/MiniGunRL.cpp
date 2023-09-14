// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "MiniGunRL.h"
#include "Main/NetPlayer.h"


AMiniGunRL::AMiniGunRL(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UObject>RL(TEXT("AnimMontage'/Game/Models/Weapons/CRL/RL.RL'"));
	fire = (UAnimMontage*)RL.Object;

	static ConstructorHelpers::FObjectFinder<UObject>cshot(TEXT("AnimMontage'/Game/Models/Weapons/CRL/cshot.cshot'"));
	chainshot = (UAnimMontage*)cshot.Object;

	static ConstructorHelpers::FObjectFinder<UObject>endshot(TEXT("AnimMontage'/Game/Models/Weapons/CRL/endshot.endshot'"));
	endcshot = (UAnimMontage*)endshot.Object;

	rl_shoot = UTIL.GetSound("weapons/machinegun", "rl_shoot");

	static ConstructorHelpers::FObjectFinder<UObject>MgunBullets(TEXT("SoundCue'/Game/Sounds/weapons/machinegun/Cue/MgunBullets.MgunBullets'"));
	Bullets = (USoundCue*)MgunBullets.Object;

	static ConstructorHelpers::FObjectFinder<UObject>MgunNoBullets(TEXT("SoundCue'/Game/Sounds/weapons/machinegun/Cue/MgunNoBullets.MgunNoBullets'"));
	NoBullets = (USoundCue*)MgunNoBullets.Object;
	
	static ConstructorHelpers::FObjectFinder<UObject>Stop(TEXT("SoundCue'/Game/Sounds/weapons/machinegun/Cue/Stop.Stop'"));
	MGunStop = (USoundCue*)Stop.Object;

	static ConstructorHelpers::FObjectFinder<UClass>Projectile(TEXT("Class'/Script/PK.MiniGunFire'"));
	AltProjectileClass = (UClass*)Projectile.Object;
	
	WeaponType = EWeapons::MiniGunRL;
}

void AMiniGunRL::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AMiniGunRL::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AMiniGunRL::Fire()
{
	if (!Super::Fire()) return false;
		
	OnFinishAltFire();
	
	if (SpawnProjectile(ProjectileClass)) /*actual shot*/
	{
		/*rocket jump*/
		Cast<UPKCharacterMovementComponent>(PLAYER->GetMovementComponent())->ScheduleRocketJump(ProjectileClass);
		/*anim*/
		PLAY(fire);
		/*shot sound*/
		UGameplayStatics::PlaySoundAttached(rl_shoot, GetRootComponent());

		return true;
	}
	return false;
}

bool AMiniGunRL::OnFinishFire()
{
	if (!Super::OnFinishFire()) return false;

	return true;
}

bool AMiniGunRL::AltFire()
{
	if (!Super::AltFire()) return false;
	
	if (SpawnProjectile(AltProjectileClass))
	{
		bAltFireOn = true;

		AudioComponent->Stop();
		AudioComponent->SetSound(Bullets);
		AudioComponent->Play();

		PLAY(chainshot);

		return true;
	}
	return false;
}

bool AMiniGunRL::OnFinishAltFire()
{
	if (!Super::OnFinishAltFire()) return false;

	if (bAltFireOn)
	{
		bAltFireOn = false;

		AudioComponent->Stop();
		AudioComponent->SetSound(MGunStop);
		AudioComponent->Play();

		PLAY(endcshot);

		/*PLAYER->MinigunFire(false);*/
		PLAYER->StopMinigunAltFire();
	}

	return true;
}

