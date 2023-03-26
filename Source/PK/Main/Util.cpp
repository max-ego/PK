// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "Util.h"

Util::Util()
{
}

Util::~Util()
{
}

void Util::AddMessage(FString s, bool overwrite, float time){
//#if UE_BUILD_DEBUG
	if (GEngine)
	{
		if (overwrite){
			const int32 MyNumberKey = 0; // Not passing -1 so each time through we will update the existing message instead  
			// of making a new one  
			GEngine->AddOnScreenDebugMessage(MyNumberKey, time, FColor::Yellow, s);
		}
		else{
			const int32 AlwaysAddKey = -1; // Passing -1 means that we will not try and overwrite an   
			// existing message, just add a new one  
			GEngine->AddOnScreenDebugMessage(AlwaysAddKey, time, FColor::Yellow, s);
		}
	}
//#endif
}

USoundBase* Util::GetSound(FString BasePath, FString SoundName)
{
	FString Path = TEXT("/Game/Sounds/") + BasePath;
	USoundBase* sound = CACHE.WrongSound;
	if (CACHE.Sounds.Contains(Path)){
		TMap<FString, USoundBase*> Sounds = CACHE.Sounds.FindChecked(Path);
		if (Sounds.Contains(SoundName)){
			USoundBase* snd = Sounds.FindChecked(SoundName);
			if (snd != NULL)
				sound = snd;
		}
	}
	return sound;
}

void Util::PlaySnd(UAudioComponent* AudioComponent, USoundCue* SoundCue)
{
	AudioComponent->Stop();
	AudioComponent->SetSound(SoundCue);
	AudioComponent->Play();
}

UUserWidget* Util::ShowWidgetClassOf(UObject* WorldContextObject, TSubclassOf<class UUserWidget> WidgetClass, bool bGrabFocus)
{
	class UUserWidget* Widget = nullptr;
	// RemoveFromViewport
	TArray<UUserWidget*> Widgets;
	FindAllObjectsClassOf(Widgets);
	for (auto* widget : Widgets){
		widget->RemoveFromViewport();
		if (WidgetClass != nullptr && widget->IsA(WidgetClass)) Widget = widget;
		//delete widget;
	}
	// AddToViewport
	if (WidgetClass != nullptr)
	{
		if (Widget == nullptr)
			Widget = CreateWidget<class UUserWidget>(GEngine->GetWorldFromContextObject(WorldContextObject), WidgetClass);
		if (Widget != nullptr)
			Widget->AddToViewport();
		
		if (bGrabFocus) Widget->SetKeyboardFocus();
	}

	return Widget;
}

void Util::RadiusRandom2D(float radius, float &x, float &y)
{
	radius *= 10;
	int32 a = FMath::RandRange(-radius, radius);
	int32 r = radius - FMath::Abs(a) / 2;
	int32 b = FMath::RandRange(-radius, radius);
	if (FMath::RandRange(0, 1) == 0)
	{
		x = a / 10.f; y = b / 10.f;
	}
	else{
		y = a / 10.f; x = b / 10.f;
	}
}

uint32 Util::GetIPAddr()
{
	uint32 LocalIPAddr = 0x7f000001; // 127.0.0.1

	return LocalIPAddr;
}

int32 Util::GetCurrentInputMode(const APlayerController *PlayerController)
{
	if (IsValid(PlayerController))
	{
		UGameViewportClient* GameViewportClient = PlayerController->GetWorld()->GetGameViewport();
		bool ignore = GameViewportClient->IgnoreInput();
		EMouseCaptureMode::Type capt = GameViewportClient->CaptureMouseOnClick();

		if (ignore == false && capt == EMouseCaptureMode::CaptureDuringMouseDown)
		{
			return 0;  // GameAndUI
		}
		else if (ignore == true && capt == EMouseCaptureMode::NoCapture)
		{
			return 1;  // UIOnly
		}
		else
		{
			return 2;  // GameOnly
		}
	}
	return -1;
}

Util UTIL = Util();
