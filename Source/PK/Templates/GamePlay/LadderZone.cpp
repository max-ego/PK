// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "LadderZone.h"
#include "Main/Util.h"
#include "Main/NetPlayer.h"


// Sets default values
ALadderZone::ALadderZone()
{
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));

	BeginOverlapDelegate.BindUFunction(this, FName("OnBeginOverlap"));
	EndOverlapDelegate.BindUFunction(this, FName("OnEndOverlap"));

	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	CollisionComp->InitBoxExtent(FVector(100.0f, 100.0f, 100.0f));

	CollisionComp->OnComponentBeginOverlap.Add(BeginOverlapDelegate);
	CollisionComp->OnComponentEndOverlap.Add(EndOverlapDelegate);

	// Set as root component
	RootComponent = Scene;
	CollisionComp->AttachParent = Scene;
}

// Called when the game starts or when spawned
void ALadderZone::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALadderZone::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ALadderZone::OnBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool fromSweep, const FHitResult& Hit)
{
	if (OtherActor && OtherActor->IsA(ANetPlayer::StaticClass())){
		Cast<ANetPlayer>(OtherActor)->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
	}
}

void ALadderZone::OnEndOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor->IsA(ANetPlayer::StaticClass())){
		Cast<ANetPlayer>(OtherActor)->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	}
}