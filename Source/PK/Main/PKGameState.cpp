// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "PKGameState.h"
#include "NetPlayerController.h"
#include "PKPlayerState.h"
#include "UnrealNetwork.h"
#include "Misc/DefaultValueHelper.h"

APKGameState::APKGameState(const FObjectInitializer& ObjectInitializer)
: AGameState(ObjectInitializer)
{

}

void APKGameState::DefaultTimer()
{
	AGameState::DefaultTimer();

	OnDefaultTimer.Broadcast();

	if (HasAuthority())
	{
		UWorld* World = GetWorld();
		if (World != NULL && !HasMatchEnded())
		{
			// Moved to PlayerController (no replication needed)
			int32 TimeLeft = TimeLimit - ElapsedTime;
			if (TimeLeft == 12)
			{
				Count = 0;
				EndOfMatchCountdown();
			}

			if (ElapsedTime == TimeLimit)
			{
				Cast<UPKGameInstance>(World->GetGameInstance())->LoadNextMap();
			}
		}

		if (FMath::Fmod(ElapsedTime, 60) == 0) SyncElapsedTime = ElapsedTime;
	}
}

void APKGameState::OnRep_SyncElapsedTime()
{
	ElapsedTime = SyncElapsedTime;
}

void APKGameState::EndOfMatchCountdown()
{
	if (/*ElapsedTime < TimeLimit*/Count < 12)
	{
		GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &APKGameState::EndOfMatchCountdown, 1.0f, false);
	}

	/*
	00001001	bellbig tick
	00000010	tock
	00000101	bell tick
	00000010	tock
	*/
	Count++;
	uint8 tick = Count & 0x01;
	uint8 tock = (!tick << 1);
	uint8 bell = ((Count & 0x01) && (Count & 0x02)) << 2;
	uint8 bellbig = (tick ^ (bell >> 2)) << 3;
	uint8 Flags = bellbig | bell | tock | tick;

	SendClockFlags(Flags);
}

void APKGameState::SendClockFlags(uint8 Flags)
{
	for (auto PlayerState : PlayerArray)
	{
		UPlayer* Player = PlayerState->GetNetOwningPlayer();
		if (Player)
		{
			ANetPlayerController* PC = Cast<ANetPlayerController>(Player->PlayerController);
			if (PC)
			{
				PC->ClockFlags = PC->ClockFlags ^ Flags;
				if (PC->IsLocalController()) PC->OnRep_ClockFlags();
			}
		}
	}
}

TArray<APlayerState*> APKGameState::GetPlayerStatesSortedById()
{
	TMap<int, int32> raw;
	for (int i = 0; i < PlayerArray.Num(); i++){
		raw.Add(i, PlayerArray[i]->PlayerId);
	}

	raw.ValueSort([](int32 A, int32 B) {
		return A < B;
	});

	TArray<int> Keys;
	raw.GetKeys(Keys);
	TArray<APlayerState*> sorted;
	for (auto key : Keys){
		sorted.Add(PlayerArray[key]);
	}
	return sorted;
}

uint16 APKGameState::GetIdxFromPlayerStates(APlayerState* PlayerState)
{
	TArray<APlayerState*> PlayerStates = GetPlayerStatesSortedById();
	uint16 idx = 0;
	for (auto state : PlayerStates)
	{
		if (PlayerState->PlayerId == state->PlayerId) break;
		idx++;
	}
	return idx;
}

ANetPlayer* APKGameState::GetPlayerByFlag(uint16 Flag)
{
	TArray<APlayerState*> PlayerStates = GetPlayerStatesSortedById();

	TArray<ANetPlayer*> Pawns;
	UTIL.FindAllObjectsClassOf(Pawns);
	
	uint8 idx = 0;
	while (Flag != 0)
	{
		idx++;
		Flag >>= 1;
	}

	for (auto pawn : Pawns)
	{
		if (pawn->IsPendingKill() || !pawn->PlayerState) continue;
		if (pawn->PlayerState->PlayerId == PlayerStates[idx-1]->PlayerId)
		{
			return pawn;
		}
	}

	return NULL;
}

void APKGameState::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	if (HasAuthority()) UpdateMatchTimeLimit();
}

void APKGameState::SetMatchTimeLimit()
{
	FString Opt = FString::Printf(TEXT("Time=%i"), TimeLimit / 60);
	FURL URL; URL.AddOption(*Opt);
	URL.SaveURLConfig(TEXT("MatchTimeLimit"), TEXT("Time"), GGameIni);
}

void APKGameState::UpdateMatchTimeLimit()
{
	FURL URL;
	URL.LoadURLConfig(TEXT("MatchTimeLimit"), GGameIni);
	const TCHAR* c = URL.GetOption(TEXT("Time="), TEXT("20")); // 20 minutes by default
	FDefaultValueHelper::ParseInt(c, TimeLimit); TimeLimit *= 60;
	/*TimeLimit = (int32)(60 * _wtoi(c));*/
}

void APKGameState::SayToAllWhoseFrag(const class UDamageType* DamageType, class AController* PC, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (!(InstigatedBy && InstigatedBy->PlayerState && PC->PlayerState && DamageType->IsA(UPKDamageType::StaticClass()))) return;
	
	FString msg = "";
	uint8 rand = 1;
	
	const class UPKDamageType* DmgType = Cast<UPKDamageType>(DamageType);
	FString KILLER = InstigatedBy->PlayerState->GetHumanReadableName();
	FString PLAYER = PC->PlayerState->GetHumanReadableName();

	bool bEnvironmentalDamage = DmgType->CausedBy == 
		EDamageCause::outofworld ||
		DmgType->CausedBy == EDamageCause::gravity ||
		DmgType->CausedBy == EDamageCause::lava;

	bool bSuicide = false;

	if (InstigatedBy == PC && !bEnvironmentalDamage)
	{
		rand = FMath::RandRange(1, 2);
		if (rand == 1)
			msg = PLAYER + " forgot which way the barrel goes.";
		else if (rand == 2)
			msg = PLAYER + " has proven that life is short.";

		bSuicide = true;
	}
	else
	{
		switch (DmgType->CausedBy)
		{
		case EDamageCause::undefined:
			msg = PLAYER + " wasted.";
			break;
		case EDamageCause::painhead:
			msg = KILLER + " shredded " + PLAYER + ".";
			break;
		case EDamageCause::painrotor:
			msg = KILLER + " shredded " + PLAYER + ".";
			break;
		case EDamageCause::painbeam:
			msg = KILLER + " shredded " + PLAYER + ".";
			break;
		case EDamageCause::stake:
			msg = KILLER + " crucified " + PLAYER + ".";
			break;
		case EDamageCause::grenade:
			msg = PLAYER + " hugged " + KILLER + "'s grenade.";
			break;
		case EDamageCause::shell:
			rand = FMath::RandRange(1, 2);
			if (rand == 1)
				msg = KILLER + " violated " + PLAYER + " with the shotgun.";
			else if (rand == 2)
				msg = KILLER + " ended " + PLAYER + "'s misery.";
			break;
		case EDamageCause::icebullet:
			UTIL.AddMessage(L"icebullet", 0, 0, 5);
			break;
		case EDamageCause::rocket:
			rand = FMath::RandRange(1, 2);
			if (rand == 1)
				msg = PLAYER + " rode " + KILLER + "'s rocket.";
			else if (rand == 2)
				msg = PLAYER + " swallowed " + KILLER + "'s rocket.";
			break;
		case EDamageCause::minigun:
			msg = KILLER + " ventilated " + PLAYER + ".";
			break;
		case EDamageCause::shuriken:
			msg = KILLER + " decorated " + PLAYER + " with the shurikens.";
			break;
		case EDamageCause::electro:
			msg = PLAYER + " was fried by " + KILLER + ".";
			break;
		case EDamageCause::telefrag:
			msg = KILLER + " skillfully telefragged " + PLAYER + ".";
			break;
		case EDamageCause::outofworld:
			msg = PLAYER + " has left the building.";
			break;
		case EDamageCause::gravity:
			msg = PLAYER + " fought gravity and lost.";
			break;
		case EDamageCause::lava:
			msg = PLAYER + " took a hot bath.";
			break;
		case EDamageCause::explosion:
			msg = PLAYER + " exploded.";
			break;
		}
	}

	for (auto PlayerState : PlayerArray)
	{
		APKPlayerState* ps = Cast<APKPlayerState>(PlayerState);
		ps->AddConsoleMessage(msg);

		// message on KILLER's Hud
		if (InstigatedBy->PlayerState == ps && !(bSuicide || bEnvironmentalDamage))
		{
			ps->SetFragMessage(PLAYER);
		}
	}
}

void APKGameState::PlayerNameChangedMsg(APlayerState* PlayerState)
{
	FString msg = PlayerState->OldName + " is now " + PlayerState->GetHumanReadableName() + ".";

	for (auto ps : PlayerArray)
	{
		Cast<APKPlayerState>(ps)->AddConsoleMessage(msg);
	}
}

void APKGameState::PlayerJoinedMsg(APlayerState* NewPlayerState)
{
	FString msg = NewPlayerState->GetHumanReadableName() + " has entered the arena.";

	for (auto PlayerState : PlayerArray)
	{
		if (NewPlayerState != PlayerState)
		{
			Cast<APKPlayerState>(PlayerState)->AddConsoleMessage(msg);

			UPlayer* Player = PlayerState->GetNetOwningPlayer();
			if (Player)
			{
				ANetPlayerController* PC = Cast<ANetPlayerController>(Player->PlayerController);
				if (PC) PC->PlaySnd(ESound::PLAYER_has_entered_the_arena);
			}
		}
	}
}

void APKGameState::PlayerLeftMsg(AController* Exiting) // "hero/hero_gib3"
{
	if (Exiting->PlayerState)
	{
		FString msg = Exiting->PlayerState->GetHumanReadableName() + " chickened and left the arena.";

		for (auto PlayerState : PlayerArray)
		{
			if (Exiting->PlayerState != PlayerState)
			{
				Cast<APKPlayerState>(PlayerState)->AddConsoleMessage(msg);

				UPlayer* Player = PlayerState->GetNetOwningPlayer();
				if (Player)
				{
					ANetPlayerController* PC = Cast<ANetPlayerController>(Player->PlayerController);
					if (PC) PC->PlaySnd(ESound::PLAYER_chickened_and_left_the_arena);
				}
			}
		}
	}
}

/************************************************************
Replication List
************************************************************/
void APKGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APKGameState, TimeLimit);
	DOREPLIFETIME(APKGameState, SyncElapsedTime);
}