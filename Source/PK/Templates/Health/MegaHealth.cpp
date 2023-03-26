// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "MegaHealth.h"


// Sets default values
AMegaHealth::AMegaHealth(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UParticleSystem> Particle(TEXT("ParticleSystem'/Game/Particles/energy/energy.energy'"));
	energy = Particle.Object;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> Particle1(TEXT("ParticleSystem'/Game/Particles/energy/menergy.menergy'"));
	menergy = Particle1.Object;

	AmmoType = EAmmoType::megahealth;
	AmmoAdd = 100;
	RespawnTime = 60;
}

void AMegaHealth::HideItem(bool bHide)
{
	APickableItem::HideItem(bHide);

	if (bHide)
	{
		if (root && e1 && e2 && e3 && e4 && e5 /*&& e6*/)
		{
			root->SetActive(false);
			e1->SetActive(false);
			e2->SetActive(false);
			e3->SetActive(false);
			e4->SetActive(false);
			e5->SetActive(false);
			/*e6->SetActive(false);*/
		}
	}
	else{
		SpawnParticles();
	}
}

void AMegaHealth::SpawnParticles()
{
	root = UGameplayStatics::SpawnEmitterAttached(menergy, GetSkeletalMeshComp(), FName("root"));
	e1 = UGameplayStatics::SpawnEmitterAttached(energy, GetSkeletalMeshComp(), FName("e1"));
	e2 = UGameplayStatics::SpawnEmitterAttached(energy, GetSkeletalMeshComp(), FName("e2"));
	e3 = UGameplayStatics::SpawnEmitterAttached(energy, GetSkeletalMeshComp(), FName("e3"));
	e4 = UGameplayStatics::SpawnEmitterAttached(energy, GetSkeletalMeshComp(), FName("e4"));
	e5 = UGameplayStatics::SpawnEmitterAttached(energy, GetSkeletalMeshComp(), FName("e5"));
	/*e6 = UGameplayStatics::SpawnEmitterAttached(energyhealth, GetSkeletalMeshComp(), FName("e6"));*/
}