// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "JumpPad.h"
#include "Main/PKCharacterMovementComponent.h"


// Sets default values
AJumpPad::AJumpPad(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	/*bReplicates = false;*/
	/*bNetLoadOnClient = false;*/

	HitDelegate.BindUFunction(this, FName("OnHit"));
	BeginOverlapDelegate.BindUFunction(this, FName("OnBeginOverlap"));
		
	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	CollisionComp->InitBoxExtent(FVector(115.0f, 115.0f, 115.0f));
	CollisionComp->OnComponentHit.Add(HitDelegate);
	CollisionComp->OnComponentBeginOverlap.Add(BeginOverlapDelegate);
	
	// Set as root component
	RootComponent = CollisionComp;

	static ConstructorHelpers::FObjectFinder<UStaticMesh>mesh(TEXT("StaticMesh'/Game/Items/podest.podest'"));
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComponent->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	StaticMeshComponent->SetStaticMesh(mesh.Object);
	StaticMeshComponent->Mobility = EComponentMobility::Movable;
	StaticMeshComponent->AttachParent = CollisionComp;

	snd = UTIL.GetSound("multiplayer/environment", "jumppod");

	// note: applying AttenuationSettings here causes crash in PIE mode on 'QuitGame'
	AttenuationSettings = CreateDefaultSubobject<USoundAttenuation>(TEXT("JumpPadSoundAttenuation"));
	AttenuationSettings->Attenuation.DistanceAlgorithm = ESoundDistanceModel::ATTENUATION_NaturalSound;
	AttenuationSettings->Attenuation.dBAttenuationAtMax = -45.f;
	AttenuationSettings->Attenuation.OmniRadius = 200.f;
	AttenuationSettings->Attenuation.FalloffDistance = 6000.f;
	if (!(WITH_EDITOR || IsRunningDedicatedServer())) snd->AttenuationSettings = AttenuationSettings;
}

// Called when the game starts or when spawned
void AJumpPad::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AJumpPad::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (SndDelay > 0.f) SndDelay -= DeltaTime;
}

void AJumpPad::OnBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool fromSweep, const FHitResult& Hit)
{
	if (OtherActor->IsA(ACharacter::StaticClass())){
		ACharacter* Pawn = Cast<ACharacter>(OtherActor);
		UPKCharacterMovementComponent* CharacterMovement = Cast<UPKCharacterMovementComponent>(Pawn->GetCharacterMovement());
		if (CharacterMovement) CharacterMovement->HandleJumpPad(JumpStrength);
		
		if (snd && SndDelay <= 0.f){
			UGameplayStatics::PlaySoundAtLocation(this, snd, GetActorLocation());
			SndDelay = CharacterMovement->JumpZVelocity / -GetWorld()->GetGravityZ();
		}
	}
}

void AJumpPad::OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	
}