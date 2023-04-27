// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "PainKiller.h"
#include "Main/NetPlayer.h"
#include "Components/PoseableMeshComponent.h"


APainKiller::APainKiller(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	static ConstructorHelpers::FObjectFinder<UObject>start(TEXT("AnimMontage'/Game/Models/Weapons/PKW/pkw_startRot.pkw_startRot'"));
	startRot = (UAnimMontage*)start.Object;

	static ConstructorHelpers::FObjectFinder<UObject>end(TEXT("AnimMontage'/Game/Models/Weapons/PKW/pkw_endRot.pkw_endRot'"));
	endRot = (UAnimMontage*)end.Object;

	static ConstructorHelpers::FObjectFinder<UObject>shot(TEXT("AnimMontage'/Game/Models/Weapons/PKW/pkw_shot.pkw_shot'"));
	startShot = (UAnimMontage*)shot.Object;

	static ConstructorHelpers::FObjectFinder<UObject>back(TEXT("AnimMontage'/Game/Models/Weapons/PKW/pkw_back.pkw_back'"));
	endShot = (UAnimMontage*)back.Object;
	
	PainkillerShoot = UTIL.GetSound("weapons/painkiller", "painkiller-shoot");

	static ConstructorHelpers::FObjectFinder<UObject>RotorStartLoop(TEXT("SoundCue'/Game/Sounds/weapons/painkiller/Cue/RotorStartLoop.RotorStartLoop'"));
	PainRotorStartLoop = (USoundCue*)RotorStartLoop.Object;

	static ConstructorHelpers::FObjectFinder<UObject>RotorStop(TEXT("SoundCue'/Game/Sounds/weapons/painkiller/Cue/RotorStop.RotorStop'"));
	PainRotorStop = (USoundCue*)RotorStop.Object;

	static ConstructorHelpers::FObjectFinder<UObject>PHShot(TEXT("SoundCue'/Game/Sounds/weapons/painkiller/Cue/PHShot.PHShot'"));
	PainHeadShot = (USoundCue*)PHShot.Object;

	static ConstructorHelpers::FObjectFinder<UObject>PHBack(TEXT("SoundCue'/Game/Sounds/weapons/painkiller/Cue/PHBack.PHBack'"));
	PainHeadBack = (USoundCue*)PHBack.Object;
	
	WeaponType = EWeapons::PainKiller;

	FName table[] = {
		TEXT("joint11|joint9"),
		TEXT("joint12|joint9"),
		TEXT("joint13|joint9"),
		TEXT("joint15")
	};
	BonesToHide.Append(table, ARRAY_COUNT(table));
}

void APainKiller::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

void APainKiller::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void APainKiller::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool APainKiller::Fire()
{
	if (!Super::Fire()) return false;
	
	if (!bPainHeadShot)
	{
		SpawnProjectile(ProjectileClass);

		PLAYER->SetPKHeadOpen(true);
		PLAY(startRot);
		PLAYSND(PainRotorStartLoop);

		return true;
	}
	return false;
}

bool APainKiller::OnFinishFire()
{
	if (!Super::OnFinishFire()) return false;

	PLAYER->SetPKHeadOpen(false);

	if (!bPainHeadShot)
	{
		PLAYSND(PainRotorStop);
		PLAY(endRot);

		PLAYER->StopPainKillerFire();
	}

	return true;
}

bool APainKiller::AltFire()
{
	Super::AltFire();

	if (!bPainHeadShot)
	{
		bPainHeadShot = true;

		if (bFirePressed){
			PLAYER->StopPainKillerFire();
			SpawnProjectile(ComboProjectileClass);
		}
		else{
			SpawnProjectile(AltProjectileClass);
		}

		for (int32 i = 0; i < BonesToHide.Num(); i++)
		{
			GetWeaponMesh()->HideBone(GetWeaponMesh()->GetBoneIndex(BonesToHide[i]), PBO_None);
		}

		PLAY(startShot);
		
		PLAYSND(NULL);
		UGameplayStatics::PlaySoundAttached(PainkillerShoot, GetOwner()->GetRootComponent());
	}
	else if (!bWithdraw){
		bWithdraw = true;
		PLAYER->ReturnPainHead();
	}

	return true;
}

bool APainKiller::OnFinishAltFire()
{
	if (!Super::OnFinishAltFire()) return false;
	return true;
}

void APainKiller::OnHeadBack()
{	
	GetWorldTimerManager().SetTimer(TimerHandle, this, &APainKiller::Reload, 0.33f, false);
	
	PLAY(endShot);
	PLAYSND(PainHeadBack);
}

void APainKiller::Reload()
{
	for (int32 i = 0; i < BonesToHide.Num(); i++)
	{
		GetWeaponMesh()->UnHideBoneByName(BonesToHide[i]);
	}
	
	bPainHeadShot = false;
	bWithdraw = false;

	if (bFirePressed) Fire();
}

void APainKiller::OnChangeWeapon()
{
	Super::OnChangeWeapon();

	if (bPainHeadShot)
	{
		if (PLAYER->Controller)
			PLAYER->ReturnPainHead();
	}
}