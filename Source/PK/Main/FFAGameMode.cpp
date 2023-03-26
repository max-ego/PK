// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "FFAGameMode.h"
#include "HUD/PKHUD.h"
#include "NetPlayer.h"
#include "NetPlayerController.h"
#include "PKGameState.h"
#include "PKPlayerState.h"


AFFAGameMode::AFFAGameMode(const FObjectInitializer& ObjectInitializer)
: APKGameMode(ObjectInitializer)
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/NetPlayer_BP"));
	
	// use our custom pawn class
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = APKHUD::StaticClass();

	// use our custom player controller
	PlayerControllerClass = ANetPlayerController::StaticClass();

	PlayerStateClass = APKPlayerState::StaticClass();

	GameStateClass = APKGameState::StaticClass();

	/* the flag is ignored for the first server player
	*  PlayerState->bOnlySpectator could be used instead, but then we have to forcibly start the match
	*/	
	bStartPlayersAsSpectators = true; //affects only the clients
}

void AFFAGameMode::StartPlay()
{
	Super::StartPlay();
}

void AFFAGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	NewPlayer->PlayerState->bOnlySpectator = bStartPlayersAsSpectators; //affects the first server player

	Cast<UPKGameInstance>(GetWorld()->GetGameInstance())->UpdateSessionSettings();

}

void AFFAGameMode::GenericPlayerInitialization(AController* C)
{
	Super::GenericPlayerInitialization(C);
}

void AFFAGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
	
	// updating SessionSettings
	Cast<UPKGameInstance>(GetWorld()->GetGameInstance())->UpdateSessionSettings();
}

void AFFAGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	if (!Exiting->IsLocalPlayerController())
		Cast<UPKGameInstance>(GetWorld()->GetGameInstance())->UpdateSessionSettings();
	
	APKGameState* GS = Cast<APKGameState>(GameState);
	if (GS && GS->IsMatchInProgress()) GS->PlayerLeftMsg(Exiting);
}