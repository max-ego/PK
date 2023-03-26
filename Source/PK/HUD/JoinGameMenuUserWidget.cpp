// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "Main/PKGameInstance.h"
#include "Main/Util.h"
#include "JoinGameMenuUserWidget.h"
#include "Internationalization/Regex.h"


UJoinGameMenuUserWidget::UJoinGameMenuUserWidget(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	FSlateColor* tableColor[] = { &ItemColor_0, &ItemColor_1, &ItemColor_2, &ItemColor_3, &ItemColor_4, &ItemColor_5, &ItemColor_6, &ItemColor_7, &ItemColor_8, &ItemColor_9, &ItemColor_10, &ItemColor_11, &ItemColor_12, &ItemColor_13, &ItemColor_14, &ItemColor_15, &ItemColor_16, &ItemColor_17, &ItemColor_18 };
	TableItemColors.Append(tableColor, ARRAY_COUNT(tableColor));

	FSlateColor* ServerIPClr[] = { &ServerIPItemColor };
	ServerIPColor.Append(ServerIPClr, ARRAY_COUNT(ServerIPClr));

	FSlateColor* CtrlClrs[] = {
		&BackButtonColor,
		&JoinButtonColor,
		&SpectateButtonColor,
		&RefreshButtonColor
	};
	ControlColors.Append(CtrlClrs, ARRAY_COUNT(CtrlClrs));

	FString* tableServer[] = { &serverItem_0, &serverItem_1, &serverItem_2, &serverItem_3, &serverItem_4, &serverItem_5, &serverItem_6, &serverItem_7, &serverItem_8, &serverItem_9, &serverItem_10, &serverItem_11, &serverItem_12, &serverItem_13, &serverItem_14, &serverItem_15, &serverItem_16, &serverItem_17, &serverItem_18 };
	FString* tableMap[] = { &mapItem_0, &mapItem_1, &mapItem_2, &mapItem_3, &mapItem_4, &mapItem_5, &mapItem_6, &mapItem_7, &mapItem_8, &mapItem_9, &mapItem_10, &mapItem_11, &mapItem_12, &mapItem_13, &mapItem_14, &mapItem_15, &mapItem_16, &mapItem_17, &mapItem_18 };
	FString* tablePlayers[] = { &playersItem_0, &playersItem_1, &playersItem_2, &playersItem_3, &playersItem_4, &playersItem_5, &playersItem_6, &playersItem_7, &playersItem_8, &playersItem_9, &playersItem_10, &playersItem_11, &playersItem_12, &playersItem_13, &playersItem_14, &playersItem_15, &playersItem_16, &playersItem_17, &playersItem_18 };
	FString* tableGame[] = { &gameItem_0, &gameItem_1, &gameItem_2, &gameItem_3, &gameItem_4, &gameItem_5, &gameItem_6, &gameItem_7, &gameItem_8, &gameItem_9, &gameItem_10, &gameItem_11, &gameItem_12, &gameItem_13, &gameItem_14, &gameItem_15, &gameItem_16, &gameItem_17, &gameItem_18 };
	FString* tablePing[] = { &pingItem_0, &pingItem_1, &pingItem_2, &pingItem_3, &pingItem_4, &pingItem_5, &pingItem_6, &pingItem_7, &pingItem_8, &pingItem_9, &pingItem_10, &pingItem_11, &pingItem_12, &pingItem_13, &pingItem_14, &pingItem_15, &pingItem_16, &pingItem_17, &pingItem_18 };
	FString* tableIP[] = { &ipItem_0, &ipItem_1, &ipItem_2, &ipItem_3, &ipItem_4, &ipItem_5, &ipItem_6, &ipItem_7, &ipItem_8, &ipItem_9, &ipItem_10, &ipItem_11, &ipItem_12, &ipItem_13, &ipItem_14, &ipItem_15, &ipItem_16, &ipItem_17, &ipItem_18 };

	ServerName.Append(tableServer, ARRAY_COUNT(tableServer));
	Map.Append(tableMap, ARRAY_COUNT(tableMap));
	Players.Append(tablePlayers, ARRAY_COUNT(tablePlayers));
	Game.Append(tableGame, ARRAY_COUNT(tableGame));
	Ping.Append(tablePing, ARRAY_COUNT(tablePing));
	IP.Append(tableIP, ARRAY_COUNT(tableIP));

	ServerIPButtonClickDeligate.BindUFunction(this, FName("ServerIPButtonClick"));
	BackButtonClickDeligate.BindUFunction(this, FName("BackButtonClick"));
	JoinButtonClickDeligate.BindUFunction(this, FName("JoinButtonClick"));
	SpectateButtonClickDeligate.BindUFunction(this, FName("SpectateButtonClick"));
	RefreshButtonClickDeligate.BindUFunction(this, FName("RefreshButtonClick"));
}

void UJoinGameMenuUserWidget::OnWidgetRebuilt()
{
	Super::OnWidgetRebuilt();

	if (!IsDesignTime())
	{
		Init();
	}
}

void UJoinGameMenuUserWidget::Init()
{
	ListShift = 0;
	ListSelectedItem = 0/*-1*/;
	ServerIpItemSelected = -1;

	if (ControlButtons.Num() >= 4)
	{
		ControlButtons[0]->OnClicked.AddUnique(BackButtonClickDeligate);
		ControlButtons[1]->OnClicked.AddUnique(JoinButtonClickDeligate);
		ControlButtons[2]->OnClicked.AddUnique(SpectateButtonClickDeligate);
		ControlButtons[3]->OnClicked.AddUnique(RefreshButtonClickDeligate);
	}

	if (ServerIPButtonArr.Num() >= 1)
	{
		ServerIPButtonArr[0]->OnClicked.AddUnique(ServerIPButtonClickDeligate);
	}

	RestoreServerIP();

	_UpdateServerList(DummySearchResultsArr, 1);
}

void UJoinGameMenuUserWidget::ServerIPButtonClick()
{
	if (bConsumeKeyboardInput)
	{
		ConfirmServerIPInput();
		bConsumeKeyboardInput = false;
	}
	bClearServerIP = true;
}

void UJoinGameMenuUserWidget::BackButtonClick()
{
	UWorld* World = GetWorld();
	if (World != NULL)
	{
		UPKGameInstance* GI = Cast<UPKGameInstance>(World->GetGameInstance());
		GI->ShowWidgetClassOf(GI->MultiplayerMenuWidgetClass);
	}
}

void UJoinGameMenuUserWidget::JoinButtonClick()
{
	UWorld* World = GetWorld();
	if (World != NULL)
	{
		UPKGameInstance* GI = Cast<UPKGameInstance>(World->GetGameInstance());
		GI->OnJoinButtonClick(ListSelectedItem, ServerIP);
	}
}

void UJoinGameMenuUserWidget::SpectateButtonClick()
{

}

void UJoinGameMenuUserWidget::RefreshButtonClick()
{
	_UpdateServerList(DummySearchResultsArr, 1);

	UWorld* World = GetWorld();
	if (World != NULL)
	{
		UPKGameInstance* GI = Cast<UPKGameInstance>(World->GetGameInstance());
		GI->FindOnlineGames();
	}
}

void UJoinGameMenuUserWidget::TickEvent(const FGeometry& MyGeometry, float InDeltaTime)
{
	UpdateItemColors(ListButtons, TableItemColors, ListSelectedItem, ListShift);
	ServerIpItemSelected = ListSelectedItem != -1 ? -1 : ServerIpItemSelected;
	UpdateItemColors(ServerIPButtonArr, ServerIPColor, ServerIpItemSelected, 0);
	ListSelectedItem = ServerIpItemSelected != -1 ? -1 : ListSelectedItem;

	/*Non-selectable*/
	dummy = -1;
	UpdateItemColors(ControlButtons, ControlColors, dummy, 0);
}

void UJoinGameMenuUserWidget::UpdateItemColors(TArray<UButton*> Buttons, TArray<FSlateColor*> ItemColors, int32 &SelectedItem, int32 Shift)
{
	for (int i = 0; i < Buttons.Num(); i++){
		if (SelectedItem - Shift == i){
			*ItemColors[i] = FLinearColor(0.784, 0.784, 0.784);
		}
		else{
			*ItemColors[i] = Buttons[i]->IsHovered() ? FLinearColor(1, 0, 0) : DefaultColor;
		}

		if (Buttons[i]->IsPressed())
		{
			if (Buttons[i]->GetUniqueID() != LastPressedButtonID)
			{
				LastPressedButtonID = Buttons[i]->GetUniqueID();
			}
		}
		else if (Buttons[i]->GetUniqueID() == LastPressedButtonID){
			SelectedItem = Shift + i;
		}
	}
}

void UJoinGameMenuUserWidget::_UpdateServerList(TArray<FString> SearchResultsArr, bool bSearchInProgress)
{
	int32 NumPlayers = 0;

	for (int i = 0; i < SearchResultsArr.Num(); i++){
		TArray<FString> split;
		SearchResultsArr[i].ParseIntoArray(&split, TEXT("|"), true);

		*ServerName[i] = split[1].Mid(0, 21);
		*Map[i] = split[2].Mid(0, 21);
		*Players[i] = split[3]; NumPlayers += FCString::Atoi(*split[3]);
		*Game[i] = split[4];
		*Ping[i] = split[5];
		*IP[i] = split[6];
	}
		
	SearchStatus = bSearchInProgress ? TEXT("Searching . . .") : SearchResultsArr.Num() > 0 ? TEXT("Update finished") : TEXT("Couldn't find a server");
	ServersFound = FString::Printf(L"Servers: %i, Players: %i", SearchResultsArr.Num(), NumPlayers);

	for (int i = SearchResultsArr.Num(); i < ServerName.Num(); i++){
		*ServerName[i] = TEXT("");
		*Map[i] = TEXT("");
		*Players[i] = TEXT("");
		*Game[i] = TEXT("");
		*Ping[i] = TEXT("");
		*IP[i] = TEXT("");
	}
}

void UJoinGameMenuUserWidget::UpdateItemColors()
{
	for (int i = 0; i < BufferColors.Num(); i++){
		if (i<TableItemColors.Num())
			*TableItemColors[i] = BufferColors[i];
	}
}

void UJoinGameMenuUserWidget::UpdateItems()
{
	SessionName.Empty();

	for (int i = 0; i < BufferItems.Num(); i++){
		TArray<FString> split;
		BufferItems[i].ParseIntoArray(&split, TEXT("|"), true);

		SessionName.Add(split[0]);

		*ServerName[i] = split[1].Mid(0,21);
		*Map[i] = split[2].Mid(0, 21);
		*Players[i] = split[3];
		*Game[i] = split[4];
		*Ping[i] = split[5];
		*IP[i] = split[6];
	}
	BufferItems.Empty();
}

FEventReply UJoinGameMenuUserWidget::OnKeyChar_Implementation(FGeometry MyGeometry, FCharacterEvent InCharacterEvent)
{
	FEventReply Reply = Super::OnKeyChar_Implementation(MyGeometry, InCharacterEvent);

	// colon character - ":"
	if (InCharacterEvent.GetCharacter() == 58 && ServerIP.Len()<21)
	{
		ServerIP.AppendChar(char(InCharacterEvent.GetCharacter()));
	}

	return Reply;
}

FEventReply UJoinGameMenuUserWidget::OnKeyDown_Implementation(FGeometry MyGeometry, FKeyEvent InKeyEvent)
{
	FEventReply Reply = Super::OnKeyDown_Implementation(MyGeometry, InKeyEvent);

	if (ServerIpItemSelected == -1) return Reply/*FEventReply(true)*/;

	bConsumeKeyboardInput = true;

	if (bClearServerIP) {
		ServerIP = "";
		bClearServerIP = false;
	}

	uint32 key = InKeyEvent.GetKeyCode();
	if (ServerIP.Len()<21 && (
		(key >= 48 && key <= 57) ||		//numbers
		(key >= 96 && key <= 105) ||	//numbers
		(InKeyEvent.GetKey() == FKey("Decimal")) ||	//delimiter
		(InKeyEvent.GetKey() == FKey("Period"))		//delimiter
		/*key == 110 || key == 190*/
		/*Left/Right Shift (160 / 161) + FKey("Semicolon")*/
		))
		ServerIP.AppendChar(char(InKeyEvent.GetCharacter()));

	switch (key)
	{
	case 27: /*Esc*/
		RestoreServerIP();
		break;
	case 13: /*Enter*/ // case is 'forwarded' to OnClicked event (ServerIPButtonClickDeligate)
		break;
	case 8: /*BackSpace*/
		ServerIP = ServerIP.Mid(0, ServerIP.Len() - 1);
		break;
	default:
		break;
	}

	return Reply/*FEventReply(true)*/;
}

void UJoinGameMenuUserWidget::ConfirmServerIPInput()
{
	FString SourceString = FString
	("^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)(\\:([0-9]{1,4}|[1-5][0-9]{4}|6[0-4][0-9]{3}|65[0-4][0-9]{2}|655[0-2][0-9]|6553[0-5]))?$");
	FRegexPattern pattern = FRegexPattern(SourceString);
	if (FRegexMatcher(pattern, ServerIP).FindNext())
		StoreServerIP();
	else
		RestoreServerIP();
}

void UJoinGameMenuUserWidget::StoreServerIP()
{
	FString Opt = FString::Printf(TEXT("IPAddress=%s"), *ServerIP);
	FURL URL; URL.AddOption(*Opt);
	URL.SaveURLConfig(TEXT("ServerIP"), TEXT("IPAddress"), GGameIni);
}

void UJoinGameMenuUserWidget::RestoreServerIP()
{
	FURL URL;
	URL.LoadURLConfig(TEXT("ServerIP"), GGameIni);
	ServerIP = URL.GetOption(TEXT("IPAddress="), TEXT("127.0.0.1"));
}

FString UJoinGameMenuUserWidget::GetServerIP()
{
	return ServerIP;
}