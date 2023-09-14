// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "DestroyPackBase.h"


// Sets default values
ADestroyPackBase::ADestroyPackBase(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	Radius = 500.f;
	DestructibleDamage = 10.f;
	ImpulseStrength = 20000.f;

	bReplicates = false;

	GetDestructibleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GetDestructibleComponent()->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_Yes;
	GetDestructibleComponent()->SetSimulatePhysics(true);

	FireDestructionTimerDelegate.BindUFunction(this, FName("FireDestruction"));
}

// Called when the game starts or when spawned
void ADestroyPackBase::BeginPlay()
{
	Super::BeginPlay();

	UWorld* const World = GetWorld();

	check(World);

	RadialForceActor = World->SpawnActorDeferred<ARadialForceActor>(ARadialForceActor::StaticClass(), GetActorLocation(), GetActorRotation());
	URadialForceComponent* RadialForceComp = RadialForceActor->GetForceComponent();
	RadialForceComp->RemoveObjectTypeToAffect(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	RadialForceComp->ForceStrength = 0.f;
	RadialForceComp->DestructibleDamage = DestructibleDamage;
	RadialForceComp->ImpulseStrength = ImpulseStrength;
	RadialForceComp->Radius = Radius;

	RadialForceActor->DisableForce();
	UGameplayStatics::FinishSpawningActor(RadialForceActor, RadialForceActor->GetTransform());
	
	GetWorldTimerManager().SetTimerForNextTick(FireDestructionTimerDelegate);

	SetLifeSpan(10.f);
}

void ADestroyPackBase::FireDestruction()
{
	if (RadialForceActor)
	{
		RadialForceActor->FireImpulse();
		RadialForceActor->SetLifeSpan(2.f);
	}
}