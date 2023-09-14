// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "PlayerSettingsMenuUserWidget.h"
#include "Main/Util.h"
#include "Main/PKGameInstance.h"

UPlayerSettingsMenuUserWidget::UPlayerSettingsMenuUserWidget(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

void UPlayerSettingsMenuUserWidget::OnWidgetRebuilt()
{
	if (!IsDesignTime())
	{
		GetNickName();
	}

	Super::OnWidgetRebuilt();
}

FEventReply UPlayerSettingsMenuUserWidget::OnKeyChar_Implementation(FGeometry MyGeometry, FCharacterEvent InCharacterEvent)
{
	uint32 key = InCharacterEvent.GetCharacter();

	if (PlayerNickName.Len()<20 &&
		!(key == 8 || key == 13 || key == 27)
		) PlayerNickName.AppendChar(InCharacterEvent.GetCharacter());

	switch (key)
	{
	case 27: /*Esc*/
		GetNickName();
		break;
	case 13: /*Enter*/
		SetNickName();
		break;
	case 8: /*BackSpace*/
		PlayerNickName = PlayerNickName.Mid(0, PlayerNickName.Len() - 1);
		break;
	default:
		break;
	}

	return FEventReply(true);
}

FEventReply UPlayerSettingsMenuUserWidget::OnKeyDown_Implementation(FGeometry MyGeometry, FKeyEvent InKeyEvent)
{
	return Super::OnKeyDown_Implementation(MyGeometry, InKeyEvent);	
	
	/*CONSUMING KEYBOARD INPUT*/

	uint32 key = InKeyEvent.GetKeyCode();
	if (PlayerNickName.Len()<20 && (
		(key >= 48 && key <= 57) || (key >= 96 && key <= 105) || // numbers (0123456789)
		(InKeyEvent.GetKey() == FKey("Decimal")) || (InKeyEvent.GetKey() == FKey("Period")) || // delimiters (. ,)
		(key >= 65 && key <= 90) || // A-Z
		(key == 189 || key == 187) || (key >= 106 && key <= 108)
		/*key == 110 || key == 190*/)
		)
		PlayerNickName.AppendChar(TCHAR(InKeyEvent.GetCharacter()));

	switch (key)
	{
	case 27: /*Esc*/
		GetNickName();
		break;
	case 13: /*Enter*/
		SetNickName();
		break;
	case 8: /*BackSpace*/
		PlayerNickName = PlayerNickName.Mid(0, PlayerNickName.Len() - 1);
		break;
	default:
		break;
	}

	return FEventReply(true);
	
}

void UPlayerSettingsMenuUserWidget::GetNickName()
{
	UWorld* World = GetWorld();
	if (World != NULL)
	{
		PlayerNickName = Cast<UPKGameInstance>(World->GetGameInstance())->GetPlayerNickName();
	}
}

void UPlayerSettingsMenuUserWidget::SetNickName()
{
	UWorld* World = GetWorld();
	if (World != NULL)
	{
		Cast<UPKGameInstance>(World->GetGameInstance())->UpdatePlayerNickName(PlayerNickName);
	}
}
