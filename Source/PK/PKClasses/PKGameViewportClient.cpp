// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "PKGameViewportClient.h"
#include "Main/Util.h"
#include "Engine/Console.h"


void UPKGameViewportClient::PostRender(UCanvas* Canvas) {

	Super::PostRender(Canvas);

	// Fade if requested, you could use the same DrawScreenFade method from any canvas such as the HUD
	if (bFading)
	{
		DrawScreenFade(Canvas);
	}
}

void UPKGameViewportClient::ClearFade()
{
	bFading = false;
}

void UPKGameViewportClient::Fade(const float Duration, const bool bToBlack, const float Delay)
{
	const UWorld* World = GetWorld();
	if (World)
	{
		bFading = true;
		this->bToBlack = bToBlack;
		FadeDuration = Duration;
		FadeStartTime = World->GetTimeSeconds() + Delay;
	}
}

void UPKGameViewportClient::DrawScreenFade(UCanvas* Canvas)
{
	if (bFading)
	{
		const UWorld* World = GetWorld();
		if (World)
		{
			const float Time = World->GetTimeSeconds();
			const float Alpha = FMath::Clamp((Time - FadeStartTime) / FadeDuration, 0.f, 1.f);

			// Make sure that we stay black in a fade to black
			if (Alpha == 1.f && !bToBlack)
			{
				bFading = false;
			}
			else{
				FColor OldColor = Canvas->DrawColor;
				FLinearColor FadeColor = FLinearColor::Black;
				FadeColor.A = bToBlack ? Alpha : 1 - Alpha;
				Canvas->DrawColor = FadeColor.ToFColor(true); // TheJamsh: "4.10 cannot convert directly to FColor, so need to use FLinearColor::ToFColor() :)
				Canvas->DrawTile(Canvas->DefaultTexture, 0, 0, Canvas->ClipX, Canvas->ClipY, 0, 0, Canvas->DefaultTexture->GetSizeX(), Canvas->DefaultTexture->GetSizeY());
				Canvas->DrawColor = OldColor;
			}
		}
	}
}

EMouseCursor::Type UPKGameViewportClient::GetCursor(FViewport* InViewport, int32 X, int32 Y)
{
	bool bIsPlayingMovie = false;//GetMoviePlayer()->IsMovieCurrentlyPlaying();

#if !PLATFORM_WINDOWS
	bool bIsWithinTitleBar = false;
#else
	POINT CursorPos = { X, Y };
	RECT WindowRect;

	bool bIsWithinWindow = true;

	// For Slate based windows the viewport doesnt have access to the OS window handle and shouln't need it
	bool bIsWithinTitleBar = false;
	if (InViewport->GetWindow())
	{
		ClientToScreen((HWND)InViewport->GetWindow(), &CursorPos);
		GetWindowRect((HWND)InViewport->GetWindow(), &WindowRect);
		bIsWithinWindow = (CursorPos.x >= WindowRect.left && CursorPos.x <= WindowRect.right &&
			CursorPos.y >= WindowRect.top && CursorPos.y <= WindowRect.bottom);

		// The user is mousing over the title bar if Y is less than zero and within the window rect
		bIsWithinTitleBar = Y < 0 && bIsWithinWindow;
	}

#endif
	
	/*if ((!InViewport->HasMouseCapture() && !InViewport->HasFocus()) || (ViewportConsole && ViewportConsole->ConsoleActive()))
	{
		return EMouseCursor::Default;
	}
	else */if ( (!bIsPlayingMovie) && (InViewport->IsFullscreen() || !bIsWithinTitleBar) )
	{
		if (GetWorld() && GetWorld()->GetFirstPlayerController())
		{
			return GetWorld()->GetFirstPlayerController()->GetMouseCursor();
		}

		return EMouseCursor::None;
	}

	return FViewportClient::GetCursor(InViewport, X, Y);
}

