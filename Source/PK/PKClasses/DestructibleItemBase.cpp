// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "DestructibleItemBase.h"


// Sets default values
ADestructibleItemBase::ADestructibleItemBase(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	/*RootComponent = Scene;*/

	NetUpdateFrequency = 30.f;
	bReplicates = true;

	CollisionComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	GetCollisionComp()->InitCapsuleSize(62.f, 90.f);
	GetCollisionComp()->BodyInstance.SetCollisionProfileName("NoCollision");
	RootComponent = GetCollisionComp();

	static ConstructorHelpers::FObjectFinder<UStaticMesh>mesh(TEXT("StaticMesh'/Game/Items/BarrelBig.BarrelBig'"));
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	GetStaticMeshComp()->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	GetStaticMeshComp()->SetStaticMesh(mesh.Object);
	GetStaticMeshComp()->Mobility = EComponentMobility::Movable;
	/*GetStaticMeshComp()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);*/
	GetStaticMeshComp()->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);

	GetStaticMeshComp()->AttachParent = GetRootComponent();

	OnTakeAnyDamageDeligate.BindUFunction(this, FName("TakeAnyDamage"));
	OnTakeAnyDamage.Add(OnTakeAnyDamageDeligate);
}

// Called when the game starts or when spawned
void ADestructibleItemBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADestructibleItemBase::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ADestructibleItemBase::TakeAnyDamage(float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{

}