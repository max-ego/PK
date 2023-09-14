// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "MyProjectile.h"


// Sets default values
AMyProjectile::AMyProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMyProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyProjectile::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

