// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "HurtBox.h"
#include "PKClasses/DamageTypes/LavaDamage.h"
#include "Main/Util.h"
#include "Main/NetPlayer.h"


// Sets default values
AHurtBox::AHurtBox()
{
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));

	PrimaryActorTick.bCanEverTick = false;

	HitDelegate.BindUFunction(this, FName("OnHit"));
	BeginOverlapDelegate.BindUFunction(this, FName("OnBeginOverlap"));
	EndOverlapDelegate.BindUFunction(this, FName("OnEndOverlap"));

	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	CollisionComp->InitBoxExtent(FVector(1000.0f, 1000.0f, 100.0f));
	CollisionComp->OnComponentHit.Add(HitDelegate);
	CollisionComp->OnComponentBeginOverlap.Add(BeginOverlapDelegate);
	CollisionComp->OnComponentEndOverlap.Add(EndOverlapDelegate);

	// Set as root component
	RootComponent = Scene;
	CollisionComp->AttachParent = Scene;	
	
	snd = UTIL.GetSound("actor/evilmonkv3", "evil-fire-hit");
}

// Called when the game starts or when spawned
void AHurtBox::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHurtBox::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AHurtBox::OnBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool fromSweep, const FHitResult& Hit)
{
	if (OtherActor){
		ANetPlayer* pawn = Cast<ANetPlayer>(OtherActor);
		if (pawn) pawn->LavaDamage = Damage;
		else if (snd) UGameplayStatics::PlaySoundAttached(snd, OtherActor->GetRootComponent());
	}
}

void AHurtBox::OnEndOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor->IsA(ANetPlayer::StaticClass())){
		Cast<ANetPlayer>(OtherActor)->LavaDamage = 0.f;
	}
}

void AHurtBox::OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

}