// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "Health.h"



// Sets default values
AHealth::AHealth(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>healthModel(TEXT("SkeletalMesh'/Game/Models/Items/energy.energy'"));
	GetSkeletalMeshComp()->SetSkeletalMesh(healthModel.Object);
	
	static ConstructorHelpers::FObjectFinder<UAnimationAsset>Anim(TEXT("AnimSequence'/Game/Models/Items/energy_idle.energy_idle'"));
	GetSkeletalMeshComp()->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	AnimAsset = Anim.Object;
	GetSkeletalMeshComp()->SetAnimation(AnimAsset);

	static ConstructorHelpers::FObjectFinder<UParticleSystem> Particle(TEXT("ParticleSystem'/Game/Particles/energy/energyhealth.energyhealth'"));
	energyhealth = Particle.Object;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> Particle1(TEXT("ParticleSystem'/Game/Particles/energy/energyhealth1.energyhealth1'"));
	energyhealth1 = Particle1.Object;

	AmmoType = EAmmoType::health;
	AmmoAdd = 25;
	RespawnTime = 25;
}

// Called when the game starts or when spawned
void AHealth::BeginPlay()
{
	Super::BeginPlay();

	GetSkeletalMeshComp()->PlayAnimation(AnimAsset, true);
	SpawnParticles();
}

// Called every frame
void AHealth::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHealth::HideItem(bool bHide)
{
	Super::HideItem(bHide);

	if (bHide)
	{
		if (root && e1 && e2 && e3 && e4 && e5 && e6)
		{
			root->SetActive(false);
			e1->SetActive(false);
			e2->SetActive(false);
			e3->SetActive(false);
			e4->SetActive(false);
			e5->SetActive(false);
			e6->SetActive(false);
		}
	}
	else{
		SpawnParticles();
	}
}

void AHealth::SpawnParticles()
{	
	root = UGameplayStatics::SpawnEmitterAttached(energyhealth1, GetSkeletalMeshComp(), FName("root"));
	e1 = UGameplayStatics::SpawnEmitterAttached(energyhealth, GetSkeletalMeshComp(), FName("e1"));
	e2 = UGameplayStatics::SpawnEmitterAttached(energyhealth, GetSkeletalMeshComp(), FName("e2"));
	e3 = UGameplayStatics::SpawnEmitterAttached(energyhealth, GetSkeletalMeshComp(), FName("e3"));
	e4 = UGameplayStatics::SpawnEmitterAttached(energyhealth, GetSkeletalMeshComp(), FName("e4"));
	e5 = UGameplayStatics::SpawnEmitterAttached(energyhealth, GetSkeletalMeshComp(), FName("e5"));
	e6 = UGameplayStatics::SpawnEmitterAttached(energyhealth, GetSkeletalMeshComp(), FName("e6"));
}