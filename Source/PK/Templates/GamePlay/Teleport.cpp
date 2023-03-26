// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "Teleport.h"


// Sets default values
ATeleport::ATeleport(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bReplicates = true;
	
	PrimaryActorTick.bCanEverTick = false;

	BeginOverlapDelegate.BindUFunction(this, FName("OnBeginOverlap"));

	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	CollisionComp->InitBoxExtent(FVector(168.0f, 84.0f, 336.0f));
	CollisionComp->OnComponentBeginOverlap.Add(BeginOverlapDelegate);

	// Set as root component
	RootComponent = CollisionComp;

	snd = UTIL.GetSound("misc", "teleport");
}

// Called when the game starts or when spawned
void ATeleport::BeginPlay()
{
	Super::BeginPlay();
	
	if (DestinationPiont){
		DestLocation = DestinationPiont->GetActorLocation();
		DestOrientation = FRotator(0.f, DestinationPiont->GetActorRotation().Yaw, 0.f);

		if (HasAuthority())
		{
			FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
			SpawnParameters.Owner = this;

			TeleFrag = GetWorld()->SpawnActor<ATeleFrag>(ATeleFrag::StaticClass(), DestLocation, DestOrientation, SpawnParameters);
		}		
	}
}

// Called every frame
void ATeleport::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ATeleport::PlaySnd_Implementation(int32 PlayerId)
{
	UPKGameInstance* GI = Cast<UPKGameInstance>(GetWorld()->GetGameInstance());
	ULocalPlayer* const LocalPlayer = GI->GetFirstGamePlayer();
	if (LocalPlayer && LocalPlayer->PlayerController)
	{
		int32 Id = LocalPlayer->PlayerController->PlayerState->PlayerId;
		if (PlayerId != Id) PlayTeleportSnd();
	}
}

void ATeleport::PlayTeleportSnd()
{
	UGameplayStatics::PlaySoundAtLocation(this, snd, DestinationPiont->GetActorLocation());
	UGameplayStatics::PlaySoundAtLocation(this, snd, GetActorLocation());
}

void ATeleport::OnBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool fromSweep, const FHitResult& Hit)
{
	if (!DestinationPiont) return;
	
	if (OtherActor->IsA(ACharacter::StaticClass()))
	{
		ACharacter* player = Cast<ACharacter>(OtherActor);
		UCharacterMovementComponent* CharacterMovement = Cast<UCharacterMovementComponent>(player->GetCharacterMovement());
		
		const FRotator VelocityRotation = CharacterMovement->Velocity.GetSafeNormal2D().Rotation();
		const FRotator DeltaVelocityRotation = DestOrientation - VelocityRotation;
		
		if (player->Controller)
		{
			if (TeleFrag) TeleFrag->Activate(player);
			
			if (player->TeleportTo(DestLocation, DestOrientation)){
				Cast<APlayerController>(player->Controller)->SetControlRotation(DestOrientation);
				CharacterMovement->Velocity = CharacterMovement->Velocity.RotateAngleAxis(DeltaVelocityRotation.Yaw, FVector(0.f, 0.f, 1.f));
				
				if (HasAuthority()) PlaySnd(player->PlayerState->PlayerId); // remotely
				if (player->IsLocallyControlled()) PlayTeleportSnd(); // locally
				
				/*warp effect*/
				Cast<ANetPlayer>(player)->WarpTime = 0;
			}

			if (TeleFrag) TeleFrag->Deactivate();
		}
	}
}
