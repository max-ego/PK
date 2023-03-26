// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "HUD/PKBaseUserWidget.h"
#include "JoinGameMenuUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class PK_API UJoinGameMenuUserWidget : public UPKBaseUserWidget
{
	GENERATED_BODY()
	
public:
	UJoinGameMenuUserWidget(const FObjectInitializer& ObjectInitializer);

	FEventReply OnKeyChar_Implementation(FGeometry MyGeometry, FCharacterEvent InCharacterEvent) override;
	FEventReply OnKeyDown_Implementation(FGeometry MyGeometry, FKeyEvent InKeyEvent) override;
	
	
protected:

	virtual void OnWidgetRebuilt() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InitialValues")
	FString RefreshButtonCaption = "Searching... Please Wait";

	FSlateColor DefaultColor = FLinearColor(1.f, 0.576f, 0.24f);

	// Itemes Color
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemColors")
		FSlateColor ItemColor_0 = DefaultColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemColors")
		FSlateColor ItemColor_1 = DefaultColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemColors")
		FSlateColor ItemColor_2 = DefaultColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemColors")
		FSlateColor ItemColor_3 = DefaultColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemColors")
		FSlateColor ItemColor_4 = DefaultColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemColors")
		FSlateColor ItemColor_5 = DefaultColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemColors")
		FSlateColor ItemColor_6 = DefaultColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemColors")
		FSlateColor ItemColor_7 = DefaultColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemColors")
		FSlateColor ItemColor_8 = DefaultColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemColors")
		FSlateColor ItemColor_9 = DefaultColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemColors")
		FSlateColor ItemColor_10 = DefaultColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemColors")
		FSlateColor ItemColor_11 = DefaultColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemColors")
		FSlateColor ItemColor_12 = DefaultColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemColors")
		FSlateColor ItemColor_13 = DefaultColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemColors")
		FSlateColor ItemColor_14 = DefaultColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemColors")
		FSlateColor ItemColor_15 = DefaultColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemColors")
		FSlateColor ItemColor_16 = DefaultColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemColors")
		FSlateColor ItemColor_17 = DefaultColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemColors")
		FSlateColor ItemColor_18 = DefaultColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemColors")
		FSlateColor ServerIPItemColor = DefaultColor;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemColors")
		TArray<FSlateColor> BufferColors;

	TArray<FSlateColor*> TableItemColors;

	UFUNCTION(BlueprintCallable, Category = "ItemColors")
		void UpdateItemColors();


	// items text
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString serverItem_0 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString serverItem_1 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString serverItem_2 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString serverItem_3 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString serverItem_4 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString serverItem_5 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString serverItem_6 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString serverItem_7 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString serverItem_8 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString serverItem_9 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString serverItem_10 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString serverItem_11 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString serverItem_12 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString serverItem_13 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString serverItem_14 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString serverItem_15 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString serverItem_16 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString serverItem_17 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString serverItem_18 = FString::FString();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString mapItem_0 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString mapItem_1 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString mapItem_2 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString mapItem_3 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString mapItem_4 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString mapItem_5 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString mapItem_6 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString mapItem_7 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString mapItem_8 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString mapItem_9 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString mapItem_10 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString mapItem_11 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString mapItem_12 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString mapItem_13 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString mapItem_14 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString mapItem_15 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString mapItem_16 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString mapItem_17 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString mapItem_18 = FString::FString();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString playersItem_0 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString playersItem_1 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString playersItem_2 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString playersItem_3 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString playersItem_4 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString playersItem_5 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString playersItem_6 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString playersItem_7 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString playersItem_8 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString playersItem_9 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString playersItem_10 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString playersItem_11 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString playersItem_12 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString playersItem_13 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString playersItem_14 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString playersItem_15 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString playersItem_16 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString playersItem_17 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString playersItem_18 = FString::FString();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString gameItem_0 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString gameItem_1 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString gameItem_2 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString gameItem_3 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString gameItem_4 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString gameItem_5 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString gameItem_6 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString gameItem_7 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString gameItem_8 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString gameItem_9 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString gameItem_10 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString gameItem_11 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString gameItem_12 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString gameItem_13 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString gameItem_14 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString gameItem_15 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString gameItem_16 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString gameItem_17 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString gameItem_18 = FString::FString();

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
		FString ipItem_0 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString ipItem_1 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString ipItem_2 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString ipItem_3 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString ipItem_4 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString ipItem_5 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString ipItem_6 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString ipItem_7 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString ipItem_8 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString ipItem_9 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString ipItem_10 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString ipItem_11 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString ipItem_12 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString ipItem_13 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString ipItem_14 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString ipItem_15 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString ipItem_16 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString ipItem_17 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		FString ipItem_18 = FString::FString();


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SearchResults")
		FString ServersFound = FString();
	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SearchResults")
		FString PlayersFound = FString();*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SearchResults")
		FString SearchStatus = FString();

	// Buttons
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buttons")
		TArray<UButton*> ServerButtons;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buttons")
		TArray<UButton*> MapButtons;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buttons")
		TArray<UButton*> PlayersButtons;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buttons")
		TArray<UButton*> GameButtons;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buttons")
		TArray<UButton*> PingButtons;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buttons")
		TArray<UButton*> IPButtons;*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buttons")
		TArray<UButton*> ListButtons;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buttons")
		TArray<UButton*> ControlButtons;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buttons")
		TArray<UButton*> ServerIPButtonArr; // only one button

	// control colors
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemColor")
		FSlateColor BackButtonColor = DefaultColor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemColor")
		FSlateColor JoinButtonColor = DefaultColor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemColor")
		FSlateColor SpectateButtonColor = DefaultColor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemColor")
		FSlateColor RefreshButtonColor = DefaultColor;
	TArray<FSlateColor*> ControlColors;
	TArray<FSlateColor*> ServerIPColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemText")
		TArray<FString> BufferItems;

	UFUNCTION(BlueprintCallable, Category = "ItemText")
		void UpdateItems();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Servers")
	TArray<FString> SessionName;

	TArray<FString*> ServerName;
	TArray<FString*> Map;
	TArray<FString*> Players;
	TArray<FString*> Game;
	TArray<FString*> Ping;
	TArray<FString*> IP;

	int32 dummy = -1;
	int32 ListShift = 0; // for the future use (if the number of servers exceeds the number of items)
	int32 ListSelectedItem;
	int32 ServerIpItemSelected;
	int32 LastPressedButtonID;
	
	TScriptDelegate<FWeakObjectPtr> ServerIPButtonClickDeligate;
	TScriptDelegate<FWeakObjectPtr> BackButtonClickDeligate;
	TScriptDelegate<FWeakObjectPtr> JoinButtonClickDeligate;
	TScriptDelegate<FWeakObjectPtr> SpectateButtonClickDeligate;
	TScriptDelegate<FWeakObjectPtr> RefreshButtonClickDeligate;

	void Init();
	UFUNCTION(BlueprintCallable, Category = "Default")
	void TickEvent(const FGeometry& MyGeometry, float InDeltaTime);
	void UpdateItemColors(TArray<UButton*> Buttons, TArray<FSlateColor*> ItemColors, int32 &SelectedItem, int32 Shift);

	void StoreServerIP();
	void RestoreServerIP();
	void ConfirmServerIPInput();

protected:
	TArray<FString> DummySearchResultsArr = {};
public:
	virtual void _UpdateServerList(TArray<FString> SearchResultsArr, bool bSearchInProgress = false);

	UFUNCTION(BlueprintImplementableEvent, Category = "CustomEvents")
		virtual void UpdateServerList(/* Params*/);

	UFUNCTION(BlueprintImplementableEvent, Category = "CustomEvents")
		virtual void JoinSessionCompleted(/* Params*/);

protected:

	bool bConsumeKeyboardInput = false;
	UFUNCTION()
	void ServerIPButtonClick();
	UFUNCTION()
	void BackButtonClick();
	UFUNCTION()
	void JoinButtonClick();
	UFUNCTION()
	void SpectateButtonClick();
	UFUNCTION()
	void RefreshButtonClick();

	UFUNCTION(BlueprintCallable, Category = "Config")
	FString GetServerIP();

	// if true clear IP on start typing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Servers")
	bool bClearServerIP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Servers")
	int32 SelectedSlot;

	UPROPERTY(BlueprintReadOnly, Category = "Servers")
	FString ServerIP;
};
