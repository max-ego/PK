// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "ChatUserWidget.h"
#include "Main/Util.h"
#include "Main/PKPlayerState.h"
#include "Engine/UserInterfaceSettings.h"


UChatUserWidget::UChatUserWidget(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	FString* tableMsgs[] = { &MsgString12, &MsgString11, &MsgString10, &MsgString9, &MsgString8, &MsgString7, &MsgString6, &MsgString5, &MsgString4, &MsgString3, &MsgString2, &MsgString1, &MsgString0 };
	MSGs.Append(tableMsgs, ARRAY_COUNT(tableMsgs));

	// legacy font size - 22 : font size - 22
	static ConstructorHelpers::FObjectFinder<UFont> fontObj(TEXT("Font'/Game/Fonts/courbd.courbd'"));
	Font = fontObj.Object;
}

void UChatUserWidget::OnWidgetRebuilt()
{
	if (!IsDesignTime())
	{
		PC = Cast<ANetPlayerController>(GetOwningPlayer());
				
		Cast<APKPlayerState>(PC->PlayerState)->Chat = this;
		UpdateConsole();
	}

	Super::OnWidgetRebuilt();
}

FEventReply UChatUserWidget::OnKeyChar_Implementation(FGeometry MyGeometry, FCharacterEvent InCharacterEvent)
{	
	uint32 key = InCharacterEvent.GetCharacter();

	FString &InputString = Cast<APKPlayerState>(PC->PlayerState)->InputString;

	if (!(key == 8 || key == 13 || key == 27)) InputString.AppendChar(InCharacterEvent.GetCharacter());

	switch (key)
	{
	case 27: /*Esc*/
		/*InputString = FString(TEXT(""));*/
		InputString.Empty();
		break;
	case 13: /*Enter*/
		if (InputString.Len() > 0) PC->ConsoleSendMessage(InputString);
		InputString.Empty();
		break;
	case 8: /*BackSpace*/
		InputString = InputString.Mid(0, InputString.Len() - 1);
		break;
	default:
		break;
	}

	UpdateCommandLine();

	return FEventReply(true);
}

void UChatUserWidget::SendMsg(FString &Text)
{
	PC->ConsoleSendMessage(Text);
	Text = FString();
}

void UChatUserWidget::UpdateCommandLine()
{
	FString &InputString = Cast<APKPlayerState>(GetOwningPlayer()->PlayerState)->InputString;
	
	int32 Pos = GetTrimPos(InputString/*.Reverse()*/, Font); // Reverse() linux failed to compile (inline function 'FString::Reverse' is not defined)
	ConsoleCommandLine = ">" + InputString.Mid(InputString.Len() - Pos, InputString.Len()) + "_";
}

int32 UChatUserWidget::GetTrimPos(FString Text, UFont* Font)
{
	const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
	float Scale = GetDefault<UUserInterfaceSettings>(UUserInterfaceSettings::StaticClass())->GetDPIScaleBasedOnSize(FIntPoint(ViewportSize.X, ViewportSize.Y));
	
	int32 AvailableSpace = FMath::CeilToInt(ViewportSize.X - (Scale * 180));

	int32 TextSize = GetTextSize(Text, Font, Scale);
	int32 pos = Text.Len();
	
	int32 len = pos;
	int32 total = 0;

	FString trail = FString::FString();

	while (TextSize > AvailableSpace)
	{
		pos -= Text.Len() / 2;
		trail = Text.Mid(pos, Text.Len());
		if (trail.Len() == 0) break;
		
		Text = Text.Mid(0, pos);
		TextSize = GetTextSize(Text, Font, Scale);

		if (TextSize <= AvailableSpace)
		{
			total += pos;
			
			AvailableSpace -= TextSize;
			Text = trail;
			TextSize = GetTextSize(Text, Font, Scale);
			pos = Text.Len();
		}
	}

	return total != 0 ? total : len;
}

void UChatUserWidget::UpdateConsole()
{
	UpdateCommandLine();

	APKPlayerState* PlayerState = Cast<APKPlayerState>(PC->PlayerState);

	for (int32 i = 0; i < PlayerState->Messages.Num(); i++)
	{
		UpdMessages(PlayerState->Messages[i]);
	}
}

void UChatUserWidget::UpdMessages(FString Text)
{		
	while (true)
	{
		for (int32 i = 0; i < MSGs.Num() - 1; i++) *MSGs[i] = *MSGs[i + 1];
		int32 pos = GetTrimPos(Text, Font);
		FString trail = Text.Mid(pos, Text.Len());
		*MSGs[MSGs.Num() - 1] = Text.Mid(0, pos);
		if (trail.Len() == 0) break;
		Text = trail;
	}
}

FEventReply UChatUserWidget::OnKeyDown_Implementation(FGeometry MyGeometry, FKeyEvent InKeyEvent)
{	
	return Super::OnKeyDown_Implementation(MyGeometry, InKeyEvent);
}

int32 UChatUserWidget::GetTextSize(FString Text, UFont* Font, float Scale)
{
	FSlateFontInfo FontInfo = Font->GetLegacySlateFontInfo();
	const TSharedRef<FSlateFontMeasure> FontMeasure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
	FVector2D size = FontMeasure->Measure(Text, FontInfo, Scale);
	return FMath::CeilToInt(size.X);
}