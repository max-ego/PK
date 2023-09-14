// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "WarmUpView.h"


// Sets default values
AWarmUpView::AWarmUpView(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCameraComponent()->bConstrainAspectRatio = false;
	GetCameraComponent()->FieldOfView = 110.f;

	SetRotTimerDelegate.BindUFunction(this, FName("SetRot"));
}

// Called when the game starts or when spawned
void AWarmUpView::BeginPlay()
{
	Super::BeginPlay();
	// fucking EPIC, delayed GetActorRotation()
	GetWorldTimerManager().SetTimerForNextTick(SetRotTimerDelegate);
}

// Called every frame
void AWarmUpView::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bCanTick) return;

	UWorld* World = GEngine->GetWorldFromContextObject(this);
	float time = World ? World->GetTimeSeconds() : 0.f;

	FRotator delta = FRotator(FMath::Sin(time / 2), FMath::Sin(time), 0);

	SetActorRotation(Rot + delta);
}

void AWarmUpView::SetRot()
{ 
	Rot = GetActorRotation();
	bCanTick = true;
}
