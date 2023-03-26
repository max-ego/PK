// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "PKPlayerState.h"
#include "HUD/ChatUserWidget.h"
#include "Net/UnrealNetwork.h"
#include "Util.h"
#include "PKGameState.h"


APKPlayerState::APKPlayerState(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	//bOnlySpectator = true;

	FString tableMsgs[] = { "", "", "", "", "", "", "", "", "", "", "", "", "" };
	Messages.Append(tableMsgs, ARRAY_COUNT(tableMsgs));

	static ConstructorHelpers::FObjectFinder<UFont> fontObj(TEXT("Font'/Game/Fonts/courbd.courbd'"));
	Font = fontObj.Object;
}

void APKPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	FURL URL;
	URL.LoadURLConfig(TEXT("DefaultPlayer"), GGameIni);
	CfgNickName = URL.GetOption(TEXT("Name="), TEXT("Unnamed"));
}

void APKPlayerState::Reset()
{
	Super::Reset();

	//bOnlySpectator = true;
}

void APKPlayerState::UpdatePlayerNickName(FString NewNickName)
{
	if (!NewNickName.IsEmpty())
	{
		FString Opt = FString::Printf(TEXT("Name=%s"), *NewNickName);
		FURL URL; URL.AddOption(*Opt);
		URL.SaveURLConfig(TEXT("DefaultPlayer"), TEXT("Name"), GGameIni);

		CfgNickName = NewNickName;
		
		SetPlayerNickName(CfgNickName);
	}
}

FString APKPlayerState::GetHumanReadableName() const
{
	return PlayerNickName;
}

void APKPlayerState::BeginPlay()
{
	Super::BeginPlay();

	UPlayer* Player = GetNetOwningPlayer();
	if ((Player && Player->PlayerController && Player->PlayerController->IsLocalController())
		|| GetNetMode() == ENetMode::NM_Client)
	{
		SetPlayerNickName(CfgNickName);
	}
}

void APKPlayerState::SetPlayerNickName_Implementation(const FString& NickName)
{
	UWorld* World = GetWorld();
	check(World);
	AGameMode* const GameMode = World->GetAuthGameMode();
	APKGameState* GS = Cast<APKGameState>(GameMode->GameState);

	OldName = PlayerNickName;

	PlayerNickName = NickName;

	if ( GetWorld()->TimeSeconds < 2 )
	{
		_bHasBeenWelcomed = true;
		return;
	}

	// new player or name change
	if (_bHasBeenWelcomed)
	{
		if (GS) GS->PlayerNameChangedMsg(this);
	}
	else
	{
		_bHasBeenWelcomed = true;
		if (GS) GS->PlayerJoinedMsg(this);
	}
};

void APKPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	UPKGameInstance* GI = Cast<UPKGameInstance>(GetWorld()->GetGameInstance());
	if (GI){
		ANetPlayerController* PC = Cast<ANetPlayerController>(GI->GetFirstLocalPlayerController());
		if (PC)
		{
			PC->bLockSpawn = false;
		}
	}
}

void APKPlayerState::FragUp()
{
	Kills = Kills < 1023 ? Kills + 1 : Kills;
	uint32 res = (Kills << 20 | Deaths << 10 | Suicides);
	Score = *((float*)&res);

	LucifersCommentary(1);
}

void APKPlayerState::FragDown(bool IsSuicide)
{
	Suicides = (IsSuicide && Suicides < 1023) ? Suicides + 1 : Suicides;
	Deaths = Deaths < 1023 ? Deaths + 1 : Deaths;
	uint32 res = (Kills << 20 | Deaths << 10 | Suicides);
	Score = *((float*)&res);

	LucifersCommentary(0);
}

void APKPlayerState::LucifersCommentary(bool Up)
{
	UPlayer* Owner = GetNetOwningPlayer();
	if (Owner)
	{
		ANetPlayerController* PC = Cast<ANetPlayerController>(Owner->PlayerController);

		if (!PC) return;

		if (Up)
		{
			LuciferBadGood = LuciferBadGood < 0 ? 1 : ++LuciferBadGood;

			if (LuciferBadGood == 5)
				PC->PlaySnd(ESound::Lucifer_good);
			else if (LuciferBadGood == 10)
			{
				PC->PlaySnd(ESound::Lucifer_excellent);
				LuciferBadGood /= 2;
			}
		}
		else
		{
			LuciferBadGood = LuciferBadGood > 0 ? -1 : --LuciferBadGood;

			if (LuciferBadGood == -5)
				PC->PlaySnd(ESound::Lucifer_bad);
			else if (LuciferBadGood == -10)
			{
				PC->PlaySnd(ESound::Lucifer_verybad);
				LuciferBadGood /= 2;
			}
		}
	}
}

void APKPlayerState::AddConsoleMessage_Implementation(const FString& InMsg)
{
	FString msg = InMsg;
	if (msg.StartsWith("$"))
	{
		msg.RemoveFromStart("$");
		
		UWorld* World = GetWorld();
		if (World && !IsRunningDedicatedServer())
		{
			ULocalPlayer* const LocalPlayer = GetWorld()->GetGameInstance()->GetFirstGamePlayer();
			Cast<ANetPlayerController>(LocalPlayer->PlayerController)->PlaySnd_Implementation(ESound::wrong_place);
		}
	}

	for (int32 i = 0; i < Messages.Num() - 1; i++) Messages[i] = Messages[i + 1];
	Messages[Messages.Num() - 1] = msg;
	
	UpdateConsole();
	HudAddMessage(msg);
}

void APKPlayerState::UpdateConsole()
{
	if (
		Chat != nullptr
		&& Chat->IsValidLowLevel()
		&& !Chat->IsPendingKill()
		&& Chat->IsInViewport()
		)
	{
		(Cast<UChatUserWidget>(Chat))->UpdateConsole();
	}
}

void APKPlayerState::HudAddMessage(FString Text)
{
	if (Text.Len()>0)
	{
		// run message clearing sequence
		if (HudMsgs.Num()==0)
			GetWorldTimerManager().SetTimer(TimerHandle, this, &APKPlayerState::WipeTopLine, WipeTimeOut, false);
	}
	else return;


	UWorld* World = GetWorld();
	if (World != NULL && World->GetGameState())
	{
		int32 time = World->GetGameState()->ElapsedTime;

		if (HudMsgs.Num() < 4) // four-line buffer array
		{
			HudMsgs.Add(Text);
			MsgTime.Add(time);
		}
		else{
			for (int32 i = 0; i < HudMsgs.Num() - 1; i++)
			{
				HudMsgs[i] = HudMsgs[i + 1];
				MsgTime[i] = MsgTime[i + 1];
			}
			HudMsgs[HudMsgs.Num() - 1] = Text;
			MsgTime[MsgTime.Num() - 1] = time;
		}

		UpdateHudMsgs();
	}
}

void APKPlayerState::UpdateHudMsgs()
{
	HudMessages.Empty();
	for (int32 i = 0; i < HudMsgs.Num(); i++)
	{
		UpdHudMessages(HudMsgs[i]);
	}
}

void APKPlayerState::UpdHudMessages(FString Text)
{
	while (true)
	{
		if (HudMessages.Num() < 4) // four lines to display
			HudMessages.Add(FString());
		else{
			for (int32 i = 0; i < HudMessages.Num() - 1; i++) HudMessages[i] = HudMessages[i + 1];
		}

		int32 pos = UChatUserWidget::GetTrimPos(Text, Font);
		FString trail = Text.Mid(pos, Text.Len());
		HudMessages[HudMessages.Num() - 1] = Text.Mid(0, pos);
		if (trail.Len() == 0) break;
		Text = trail;
	}
}

void APKPlayerState::WipeTopLine()
{
	UWorld* World = GetWorld();
	if (World != NULL && World->GetGameState())
	{
		for (int32 i = 0; i < HudMsgs.Num() - 1; i++) {
			HudMsgs[i] = HudMsgs[i + 1];
			MsgTime[i] = MsgTime[i + 1];
		}

		if (HudMsgs.Num()>0)
		{
			HudMsgs.RemoveAt(HudMsgs.Num() - 1);
			MsgTime.RemoveAt(MsgTime.Num() - 1);
		}

		UpdateHudMsgs();

		if (HudMsgs.Num() == 0) return;

		int32 time = World->GetGameState()->ElapsedTime;
		float delay = WipeTimeOut - (time - MsgTime[0]);
		delay = delay <= 0 ? 0.5 : delay + 0.5;

		if (HudMessages.Num()>0) GetWorldTimerManager().SetTimer(TimerHandle, this, &APKPlayerState::WipeTopLine, delay, false);
	}	
}

void APKPlayerState::SetFragMessage_Implementation(const FString& VictimName)
{
	FragMessage = VictimName;
	GetWorldTimerManager().SetTimer(FragMsgTimerHandle, this, &APKPlayerState::ClearFragMessage, 2.f, false);
}

void APKPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APKPlayerState, PlayerNickName);
	DOREPLIFETIME(APKPlayerState, Health);

	/*DOREPLIFETIME_CONDITION(APKPlayerState, PlayerNickName, COND_SimulatedOnly);*/
}