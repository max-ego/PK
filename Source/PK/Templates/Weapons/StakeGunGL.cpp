// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "StakeGunGL.h"
#include "Main/NetPlayer.h"
#include "Main/NetPlayerController.h"


AStakeGunGL::AStakeGunGL(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UClass>Projectile(TEXT("Class'/Script/PK.TmpProjectile'"));
	ProjectileClass = (UClass*)Projectile.Object;

	static ConstructorHelpers::FObjectFinder<UObject>pinshot(TEXT("AnimMontage'/Game/Models/Weapons/KGR/pinshot.pinshot'"));
	fire = (UAnimMontage*)pinshot.Object;

	static ConstructorHelpers::FObjectFinder<UObject>GRshot(TEXT("AnimMontage'/Game/Models/Weapons/KGR/GRshot.GRshot'"));
	altfire = (UAnimMontage*)GRshot.Object;

	stake_fire = UTIL.GetSound("weapons/stake", "weapon_stake-shoot_mp");
	grenade_fire = UTIL.GetSound("weapons/grenadelauncher", "weapon_grenade_fire");
	
	WeaponType = EWeapons::StakeGunGL;
}

void AStakeGunGL::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AStakeGunGL::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AStakeGunGL::Fire()
{
	if (!Super::Fire()) return false;

	if (SpawnProjectile(ProjectileClass))
	{
		PLAY(fire);
		UGameplayStatics::PlaySoundAttached(stake_fire, GetRootComponent());

		return true;
	}
	return false;
}

void AStakeGunGL::HideStake(bool bHide)
{
	if (bHide)
	{
		GetWeaponMesh()->HideBone(GetWeaponMesh()->GetBoneIndex("k_kolek"), EPhysBodyOp::PBO_None);
	}
	else{
		GetWeaponMesh()->UnHideBone(GetWeaponMesh()->GetBoneIndex("k_kolek"));
	}
}

void AStakeGunGL::Reload()
{
	GetWeaponMesh()->UnHideBone(GetWeaponMesh()->GetBoneIndex("k_kolek"));
}

bool AStakeGunGL::OnFinishFire()
{
	if (!Super::OnFinishFire()) return false;

	return true;	
}

bool AStakeGunGL::AltFire()
{
	if (!Super::AltFire()) return false;

	if (SpawnProjectile(AltProjectileClass))
	{
		PLAY(altfire);
		UGameplayStatics::PlaySoundAttached(grenade_fire, GetRootComponent());

		return true;
	}
	return false;
}

bool AStakeGunGL::OnFinishAltFire()
{
	if (!Super::OnFinishAltFire()) return false;

	return true;
}

