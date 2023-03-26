// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "PKGameSession.h"


APKGameSession::APKGameSession(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

void APKGameSession::InitOptions(const FString& Options)
{
	Super::InitOptions(Options);

	// limit the maximum number of players to 16 due to the replicated PlayerFlag bitmask of type uint16.
	// see: APKGameState::GetPlayerByFlag
	MaxPlayers = MaxPlayers > 16 ? 16 : MaxPlayers;
}

