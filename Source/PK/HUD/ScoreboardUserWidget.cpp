// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "ScoreboardUserWidget.h"
#include "Misc/DateTime.h"
#include "Main/PKGameState.h"
#include "Main/Util.h"


UScoreboardUserWidget::UScoreboardUserWidget(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	FString* tablePlayer[] = { &playerItem_0, &playerItem_1, &playerItem_2, &playerItem_3, &playerItem_4, &playerItem_5, &playerItem_6, &playerItem_7, &playerItem_8, &playerItem_9, &playerItem_10, &playerItem_11, &playerItem_12, &playerItem_13, &playerItem_14, &playerItem_15, &playerItem_16, &playerItem_17, &playerItem_18, &playerItem_19, &playerItem_20 };	
	FString* tableFrags[] = { &fragsItem_0, &fragsItem_1, &fragsItem_2, &fragsItem_3, &fragsItem_4, &fragsItem_5, &fragsItem_6, &fragsItem_7, &fragsItem_8, &fragsItem_9, &fragsItem_10, &fragsItem_11, &fragsItem_12, &fragsItem_13, &fragsItem_14, &fragsItem_15, &fragsItem_16, &fragsItem_17, &fragsItem_18, &fragsItem_19, &fragsItem_20 };
	FString* tableKills[] = { &killsItem_0, &killsItem_1, &killsItem_2, &killsItem_3, &killsItem_4, &killsItem_5, &killsItem_6, &killsItem_7, &killsItem_8, &killsItem_9, &killsItem_10, &killsItem_11, &killsItem_12, &killsItem_13, &killsItem_14, &killsItem_15, &killsItem_16, &killsItem_17, &killsItem_18, &killsItem_19, &killsItem_20 };
	FString* tableDeaths[] = { &deathsItem_0, &deathsItem_1, &deathsItem_2, &deathsItem_3, &deathsItem_4, &deathsItem_5, &deathsItem_6, &deathsItem_7, &deathsItem_8, &deathsItem_9, &deathsItem_10, &deathsItem_11, &deathsItem_12, &deathsItem_13, &deathsItem_14, &deathsItem_15, &deathsItem_16, &deathsItem_17, &deathsItem_18, &deathsItem_19, &deathsItem_20 };
	FString* tablePing[] = { &pingItem_0, &pingItem_1, &pingItem_2, &pingItem_3, &pingItem_4, &pingItem_5, &pingItem_6, &pingItem_7, &pingItem_8, &pingItem_9, &pingItem_10, &pingItem_11, &pingItem_12, &pingItem_13, &pingItem_14, &pingItem_15, &pingItem_16, &pingItem_17, &pingItem_18, &pingItem_19, &pingItem_20 };
	FString* tableTime[] = { &timeItem_0, &timeItem_1, &timeItem_2, &timeItem_3, &timeItem_4, &timeItem_5, &timeItem_6, &timeItem_7, &timeItem_8, &timeItem_9, &timeItem_10, &timeItem_11, &timeItem_12, &timeItem_13, &timeItem_14, &timeItem_15, &timeItem_16, &timeItem_17, &timeItem_18, &timeItem_19, &timeItem_20 };

	Player.Append(tablePlayer, ARRAY_COUNT(tablePlayer));
	Frags.Append(tableFrags, ARRAY_COUNT(tableFrags));
	Kills.Append(tableKills, ARRAY_COUNT(tableKills));
	Deaths.Append(tableDeaths, ARRAY_COUNT(tableDeaths));
	Ping.Append(tablePing, ARRAY_COUNT(tablePing));
	Time.Append(tableTime, ARRAY_COUNT(tableTime));

	OnDefaultTimerDelegate.BindUFunction(this, FName("SortStats"));
}

// not used
void UScoreboardUserWidget::UpdateItems()
{
	// Unnamed|0|0|0|0|00:00
	for (int i = 0; i < BufferItems.Num(); i++){
		TArray<FString> split;
		BufferItems[i].ParseIntoArray(&split, TEXT("|"), true);

		*Player[i] = split[0].Mid(0, 15);
		*Frags[i] = split[1];
		*Kills[i] = split[2];
		*Deaths[i] = split[3];
		*Ping[i] = split[4];
		*Time[i] = split[5];
	}
	BufferItems.Empty();
}

void UScoreboardUserWidget::PopulateItems(TArray<APlayerState*> PlayerStates)
{
	for (int i = 0; i < PlayerStates.Num(); i++){

		APlayerState* state = PlayerStates[i];

		*Player[i] = state->GetHumanReadableName();
		
		uint32 in = *((int*)&state->Score);			// max value 1023
		int32 kills    = (0x3FF00000 & in) >> 20;	//00111111111100000000000000000000
		int32 deaths   = (0xFFC00 & in)    >> 10;	//00000000000011111111110000000000
		int32 suicides = (0x3FF & in)      >> 00;	//00000000000000000000001111111111
		int32 frags = kills - suicides;

		*Frags[i] = FString::FromInt(frags);
		*Kills[i] = FString::FromInt(kills);
		*Deaths[i] = FString::FromInt(deaths);

		*Ping[i] = FString::FromInt(FMath::Min(255, (int32)(state->Ping * 4)));
		*Time[i] = PlayTime(state);
	}

	for (int i = PlayerStates.Num(); i < Player.Num(); i++){
		*Player[i] = TEXT("");
		*Frags[i] = TEXT("");
		*Kills[i] = TEXT("");
		*Deaths[i] = TEXT("");
		*Ping[i] = TEXT("");
		*Time[i] = TEXT("");
	}
}

FString UScoreboardUserWidget::PlayTime(APlayerState* state)
{
	UWorld* World = GetWorld();
	if (World != NULL && World->GetGameState())
	{
		return SecondsToString(World->GetGameState()->ElapsedTime - state->StartTime);
	}
	return L"00:00";
}

void UScoreboardUserWidget::SortStats()
{
	UWorld* World = GetWorld();
	if (World != NULL && World->GetGameState())
	{
		TArray<APlayerState*> PlayerStates = World->GetGameState()->PlayerArray;

		TMap<int, int16> raw;
		for (int i = 0; i < PlayerStates.Num(); i++){
			uint32 in = *((int*)&PlayerStates[i]->Score);
			int32 kills = (0x3FF00000 & in) >> 20;	//00111111111100000000000000000000
			int32 suicides = (0x3FF & in) >> 00;	//00000000000000000000001111111111
			int32 frags = kills - suicides;
			raw.Add(i, frags);
		}

		raw.ValueSort([](int16 A, int16 B) {
			return A > B;
		});

		TArray<int> Keys;
		raw.GetKeys(Keys);
		TArray<APlayerState*> sorted;
		for (auto key : Keys){
			sorted.Add(PlayerStates[key]);
		}

		PopulateItems(sorted);

		APKGameState* GS = Cast<APKGameState>(World->GetGameState());
		if (GS)
		{
			FString Base = GS->TimeLimit - GS->ElapsedTime < 0 ? "OverTime: " : "TimeLeft: ";
			TimeLeft = Base + SecondsToString(GS->TimeLimit - GS->ElapsedTime);

			FINALRESULTSTextVisibility = GS->HasMatchEnded() ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
		}
	}	
}

FString UScoreboardUserWidget::SecondsToString(int32 time)
{
	time = FMath::Abs(time);

	FString Hrs = FString::FromInt(FMath::FloorToInt(time / 3600));
	int32 mins = FMath::FloorToInt(time / 60);
	FString Min = FString::FromInt(FMath::Fmod(mins, 60));
	FString Sec = FString::FromInt(FMath::Fmod(time, 60));

	Min = Min.Len() > 1 ? Min : L"0" + Min;
	Sec = Sec.Len() > 1 ? Sec : L"0" + Sec;

	return FMath::FloorToInt(time / 3600) > 0 ? Hrs + L":" + Min + L":" + Sec : Min + L":" + Sec;
}

void UScoreboardUserWidget::OnWidgetRebuilt()
{	
	if (!IsDesignTime())
	{
		UWorld* World = GetWorld();
		if (World != NULL && World->GetGameState())
		{
			SortStats();
			APKGameState* GameState = Cast<APKGameState>(World->GetGameState());
			GameState->OnDefaultTimer.AddUnique(OnDefaultTimerDelegate);
		}
	}

	Super::OnWidgetRebuilt();
}