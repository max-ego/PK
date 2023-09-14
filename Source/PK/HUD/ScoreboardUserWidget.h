// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "PKBaseUserWidget.h"
#include "ScoreboardUserWidget.generated.h"

/**
 * 
 */
UCLASS()
//class PK_API UScoreboardUserWidget : public UPKBaseUserWidget
class PK_API UScoreboardUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UScoreboardUserWidget(const FObjectInitializer& ObjectInitializer);


public:
	
	// items text
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString playerItem_0 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString playerItem_1 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString playerItem_2 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString playerItem_3 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString playerItem_4 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString playerItem_5 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString playerItem_6 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString playerItem_7 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString playerItem_8 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString playerItem_9 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString playerItem_10 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString playerItem_11 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString playerItem_12 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString playerItem_13 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString playerItem_14 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString playerItem_15 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString playerItem_16 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString playerItem_17 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString playerItem_18 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString playerItem_19 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString playerItem_20 = FString::FString();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString fragsItem_0 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString fragsItem_1 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString fragsItem_2 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString fragsItem_3 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString fragsItem_4 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString fragsItem_5 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString fragsItem_6 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString fragsItem_7 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString fragsItem_8 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString fragsItem_9 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString fragsItem_10 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString fragsItem_11 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString fragsItem_12 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString fragsItem_13 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString fragsItem_14 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString fragsItem_15 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString fragsItem_16 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString fragsItem_17 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString fragsItem_18 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString fragsItem_19 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString fragsItem_20 = FString::FString();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString killsItem_0 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString killsItem_1 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString killsItem_2 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString killsItem_3 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString killsItem_4 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString killsItem_5 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString killsItem_6 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString killsItem_7 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString killsItem_8 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString killsItem_9 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString killsItem_10 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString killsItem_11 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString killsItem_12 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString killsItem_13 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString killsItem_14 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString killsItem_15 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString killsItem_16 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString killsItem_17 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString killsItem_18 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString killsItem_19 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString killsItem_20 = FString::FString();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString deathsItem_0 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString deathsItem_1 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString deathsItem_2 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString deathsItem_3 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString deathsItem_4 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString deathsItem_5 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString deathsItem_6 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString deathsItem_7 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString deathsItem_8 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString deathsItem_9 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString deathsItem_10 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString deathsItem_11 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString deathsItem_12 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString deathsItem_13 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString deathsItem_14 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString deathsItem_15 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString deathsItem_16 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString deathsItem_17 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString deathsItem_18 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString deathsItem_19 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString deathsItem_20 = FString::FString();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString pingItem_0 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString pingItem_1 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString pingItem_2 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString pingItem_3 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString pingItem_4 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString pingItem_5 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString pingItem_6 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString pingItem_7 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString pingItem_8 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString pingItem_9 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString pingItem_10 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString pingItem_11 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString pingItem_12 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString pingItem_13 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString pingItem_14 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString pingItem_15 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString pingItem_16 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString pingItem_17 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString pingItem_18 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString pingItem_19 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString pingItem_20 = FString::FString();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString timeItem_0 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString timeItem_1 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString timeItem_2 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString timeItem_3 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString timeItem_4 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString timeItem_5 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString timeItem_6 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString timeItem_7 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString timeItem_8 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString timeItem_9 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString timeItem_10 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString timeItem_11 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString timeItem_12 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString timeItem_13 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString timeItem_14 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString timeItem_15 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString timeItem_16 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString timeItem_17 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString timeItem_18 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString timeItem_19 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString timeItem_20 = FString::FString();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		TArray<FString> BufferItems;

	UFUNCTION(BlueprintCallable, Category = "ItemText")
		void SortStats();

	TArray<FString*> Player;
	TArray<FString*> Frags;
	TArray<FString*> Kills;
	TArray<FString*> Deaths;
	TArray<FString*> Ping;
	TArray<FString*> Time;

	UFUNCTION(BlueprintImplementableEvent, Category = "CustomEvents")
		virtual void UpdateStats(/* Params*/);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game")
	FString TimeLeft = FString();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buttons")
	ESlateVisibility FINALRESULTSTextVisibility = ESlateVisibility::Hidden;

	void UpdateItems();
	void PopulateItems(TArray<APlayerState*> PlayerStates);

	virtual void OnWidgetRebuilt() override;

	virtual FString PlayTime(APlayerState* state);

	FString SecondsToString(int32 time);

	TScriptDelegate<FWeakObjectPtr> OnDefaultTimerDelegate;

};
