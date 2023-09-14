// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "TeleFrag.h"
#include "Main/Util.h"
#include "PKClasses/DamageTypes/TelefragDamage.h"


// Sets default values
ATeleFrag::ATeleFrag(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));

	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	GetCollisionComp()->InitSphereRadius(80.0f);

	GetCollisionComp()->BodyInstance.SetCollisionProfileName("OverlapAll");

	// Set as root component
	RootComponent = CollisionComp;

	BeginOverlapDelegate.BindUFunction(this, FName("OnBeginOverlap"));
	GetCollisionComp()->OnComponentBeginOverlap.Add(BeginOverlapDelegate);
}

// Called when the game starts or when spawned
void ATeleFrag::BeginPlay()
{
	Super::BeginPlay();
	
	Deactivate();
}

// Called every frame
void ATeleFrag::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ATeleFrag::OnBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool fromSweep, const FHitResult& Hit)
{
	if (FragInstigator == nullptr) return;

	UDamageType const* const DmgType = GetDefault<UTelefragDamage>();

	if (OtherActor->IsA(ACharacter::StaticClass()) && OtherActor != FragInstigator){
		UGameplayStatics::ApplyDamage(OtherActor, 1000, FragInstigator->Controller, this, DmgType->GetClass());
	}
}

void ATeleFrag::Activate(ACharacter* Instigator)
{
	FragInstigator = Instigator;
	
	GetCollisionComp()->BodyInstance.SetCollisionProfileName("OverlapAll");
	
	FVector Loc = GetActorLocation();
	SetActorLocation(Loc + FVector(0, 0, 1), true);
	SetActorLocation(Loc);
}

void ATeleFrag::Deactivate()
{
	FragInstigator = nullptr;

	GetCollisionComp()->BodyInstance.SetCollisionProfileName("NoCollision");
}