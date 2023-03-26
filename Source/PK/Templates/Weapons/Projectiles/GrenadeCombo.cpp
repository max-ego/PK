// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "GrenadeCombo.h"
#include "Main/Util.h"


// Sets default values
AGrenadeCombo::AGrenadeCombo()
{
	PrimaryActorTick.bCanEverTick = true;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = Scene;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComponent->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	StaticMeshComponent->bGenerateOverlapEvents = false;
	StaticMeshComponent->Mobility = EComponentMobility::Movable;
	StaticMeshComponent->AttachParent = RootComponent;
	StaticMeshComponent->SetHiddenInGame(false);

	static ConstructorHelpers::FObjectFinder<UStaticMesh>ProjectileMesh(TEXT("/Game/Items/granat.granat"));
	StaticMeshComponent->SetStaticMesh(ProjectileMesh.Object);
	StaticMeshComponent->SetRelativeLocation(FVector(-20.0f, 0.0f, 0.0f));
	StaticMeshComponent->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
}

// Called when the game starts or when spawned
void AGrenadeCombo::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGrenadeCombo::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (GetOwner() && GetOwner()->IsValidLowLevel() && !GetOwner()->IsPendingKill())
		UTIL.AddMessage(L"%.1f", (GetOwner()->GetActorLocation() - GetActorLocation()).Size(), 1, 1);
}

