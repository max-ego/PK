// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "Main/Util.h"
#include "PKLevelScriptActor.h"


APKLevelScriptActor::APKLevelScriptActor(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

	static ConstructorHelpers::FObjectFinder<UClass>PlayerAppearance_Actor(TEXT("Blueprint'/Game/HUD/Menu/Multiplayer/Appearance/PlayerAppearance_Actor.PlayerAppearance_Actor_C'"));
	PlayerAppearance = (UClass*)PlayerAppearance_Actor.Object;

	static ConstructorHelpers::FObjectFinder<UClass>SceneCapture2D(TEXT("Blueprint'/Game/HUD/Menu/Multiplayer/Appearance/PlayerAppearance_SceneCapture2D_Blueprint.PlayerAppearance_SceneCapture2D_Blueprint_C'"));
	Capture2D = (UClass*)SceneCapture2D.Object;
}

// Called when the game starts or when spawned
void APKLevelScriptActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (PlayerAppearance && Capture2D){
		GetWorld()->SpawnActor<AActor>(PlayerAppearance, FVector(0.f, 0.f, -6100.f), FRotator(0, 90, 0));
		GetWorld()->SpawnActor<ASceneCapture2D>(Capture2D, FVector(-650.f, 0.f, -6000.f), FRotator::ZeroRotator);
	}
}

void APKLevelScriptActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

}