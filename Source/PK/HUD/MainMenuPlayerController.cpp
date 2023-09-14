// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "MainMenuPlayerController.h"
#include "PKClasses/WarmUpView.h"
#include "Main/PKGameState.h"
#include "Engine/UserInterfaceSettings.h"

AMainMenuPlayerController::AMainMenuPlayerController(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	
	bShowMouseCursor = true;

	CenterCursorTimerDelegate.BindUFunction(this, FName("CenterCursor"));
}


void AMainMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetInputMode(FInputModeUIOnly());

	TArray<AActor*> WarmUpViews;
	UGameplayStatics::GetAllActorsOfClass(this, AWarmUpView::StaticClass(), WarmUpViews);
	if (WarmUpViews.Num()>0) PlayerCameraManager->SetViewTarget(WarmUpViews[0]);

	GetWorldTimerManager().SetTimer(TimerHandle, this, &AMainMenuPlayerController::CenterCursor, 0.5f, false);	
}

void AMainMenuPlayerController::Tick(float DeltaTime)
{
	if (!IsRunningDedicatedServer()){
		// Mouse Cursor not changing until moved
		// workaround
//#if PLATFORM_WINDOWS
		FSlateApplication::Get().QueryCursor();
//#endif		
	}
}

void AMainMenuPlayerController::CenterCursor()
{
	if (!IsRunningDedicatedServer())
	{
		if (GEngine != nullptr)
		{
			if (GEngine->GameUserSettings->GetFullscreenMode() != EWindowMode::Fullscreen)
			{
				FViewport* Viewport = CastChecked<ULocalPlayer>(this->Player)->ViewportClient->Viewport;
				FVector2D ViewportSize = Viewport->GetSizeXY();
				Viewport->SetMouse(ViewportSize.X / 2, ViewportSize.Y / 2);
			}
		}		
	}
}