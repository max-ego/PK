// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "PKBaseUserWidget.h"
#include "Main/Util.h"
#include "Main/PKGameState.h"



UPKBaseUserWidget::UPKBaseUserWidget(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<USoundBase> QuitAcceptSound(TEXT("SoundWave'/Game/Sounds/menu/menu/quit-accept.quit-accept'"));
	QuitAccept = QuitAcceptSound.Object;
	
	static ConstructorHelpers::FObjectFinder<USoundBase> OptionAcceptSound(TEXT("SoundWave'/Game/Sounds/menu/menu/option-accept.option-accept'"));
	OptionAccept = OptionAcceptSound.Object;

	static ConstructorHelpers::FObjectFinder<USoundBase> QuitLightOnSound(TEXT("SoundWave'/Game/Sounds/menu/menu/quit-light-on.quit-light-on'"));
	QuitLightOn = QuitLightOnSound.Object;

	static ConstructorHelpers::FObjectFinder<UClass>ScoreboardWidgetCls(TEXT("Class'/Game/HUD/ScoreBoard.Scoreboard_C'"));
	ScoreBoardWidget = (UClass*)ScoreboardWidgetCls.Object;
}

void UPKBaseUserWidget::OnWidgetRebuilt()
{
	if (!IsDesignTime())
	{		
		LastKeyCode = -1;
	}

	Super::OnWidgetRebuilt();
}

FEventReply UPKBaseUserWidget::OnKeyChar_Implementation(FGeometry MyGeometry, FCharacterEvent InCharacterEvent)
{
	return FEventReply(true);
}

FEventReply UPKBaseUserWidget::OnKeyUp_Implementation(FGeometry MyGeometry, FKeyEvent InKeyEvent)
{
	LastKeyCode = -1;

	return FEventReply(true);
}

FEventReply UPKBaseUserWidget::OnKeyDown_Implementation(FGeometry MyGeometry, FKeyEvent InKeyEvent)
{
	if (LastKeyCode == InKeyEvent.GetKeyCode()) return FEventReply(true);
	LastKeyCode = InKeyEvent.GetKeyCode();

	if (InKeyEvent.GetKeyCode() == 27)
	{
		bool bOnEscape = true;
		
		UWorld* World = GetWorld();
		if (World != NULL)
		{
			if (World->GetAuthGameMode() &&
				World->GetAuthGameMode()->IsA(AMainMenuGameMode::StaticClass()) &&
				GetClass()->GetName() == "MainMenu_C")
				return FEventReply(true);

			UPKGameInstance* GI = Cast<UPKGameInstance>(World->GetGameInstance());
			APlayerController* PC = GI->GetFirstLocalPlayerController();

			if (!GI->InterruptionConnection())
			{
				if (!PC->AcknowledgedPawn)
				{
					bOnEscape = false;
					GI->OnResume();
					UTIL.ShowWidgetClassOf(GI, ScoreBoardWidget, false);

					PlaySound(QuitLightOn);
				}
				else if (GetClass()->GetName() == "MainMenu_C")
				{
					PlaySound(OptionAccept);
				}
				else if (GetClass()->GetName() == "Chat_C")
				{
					PlaySound(QuitLightOn);
				}
				else PlaySound(QuitAccept);
			}
		}

		if (bOnEscape)
		{
			OnEscape();
		}
	}

	return FEventReply(true);
}


