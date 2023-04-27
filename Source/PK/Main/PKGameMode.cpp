// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "PK.h"
#include "PKGameMode.h"
#include "PKGameState.h"
#include "HUD/PKHUD.h"
#include "NetPlayer.h"
#include "NetPlayerController.h"
#include "PKGameSession.h"
#include "PKPlayerState.h"


APKGameMode::APKGameMode(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

	PlayerStateClass = APKPlayerState::StaticClass();
}

// provide our custom GameSession
TSubclassOf<AGameSession> APKGameMode::GetGameSessionClass() const
{
	return APKGameSession::StaticClass();
}

void APKGameMode::ChangeMenuWidget(TSubclassOf<class UUserWidget> NewWidgetClass)
{
	UTIL.ShowWidgetClassOf(this,NewWidgetClass);
}

void APKGameMode::HandleMatchAborted()
{
	Super::HandleMatchAborted();
}

void APKGameMode::AbortMatch()
{
	Super::AbortMatch();
}

APlayerController* APKGameMode::Login(UPlayer* NewPlayer, const FString& Portal, const FString& Options, const TSharedPtr<FUniqueNetId>& UniqueId, FString& ErrorMessage)
{
	return Super::Login(NewPlayer, Portal, Options, UniqueId, ErrorMessage);
}

FString APKGameMode::InitNewPlayer(APlayerController* NewPlayerController, const TSharedPtr<FUniqueNetId>& UniqueId, const FString& Options, const FString& Portal)
{
	return Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
}
