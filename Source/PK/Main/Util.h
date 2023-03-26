// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Engine.h"
#include "PKCache.h"

#include "EngineUtils.h"

/**
 * 
 */

#define D(x) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT(x));}
#define DM(x, key, ...) if(GEngine){GEngine->AddOnScreenDebugMessage(key, 5.0f, FColor::Yellow, FString::Printf(TEXT(x), __VA_ARGS__));}

class PK_API Util
{
public:
	
	Util();
	~Util();

	// Debugging message
	void AddMessage(FString s, bool overwrite = false, float time = 5.f);
	
	template<typename T>
	void AddMessage(const TCHAR* fmt, T V, bool overwrite = false, float time = 5.f)
	{
		AddMessage(FString::Printf(fmt, V), overwrite, time);
	}
	
	USoundBase* GetSound(FString BasePath, FString SoundName);
	
	UUserWidget* ShowWidgetClassOf(UObject* WorldContextObject, TSubclassOf<class UUserWidget> WidgetClass, bool bGrabFocus = true);

	template<typename T>
	void FindAllActors(UWorld* World, TArray<T*>& Out)
	{
		for (TActorIterator<AActor> It(World, T::StaticClass()); It; ++It)
		{
			T* Actor = Cast<T>(*It);
			if (Actor && !Actor->IsPendingKill())
			{
				Out.Add(Actor);
			}
		}
	}

	template<typename T>
	void FindAllObjectsClassOf(TArray<T*>& Out)
	{
		for (TObjectIterator<UObject> It; It; ++It)
		{
			T* obj = Cast<T>(*It);
			if (obj)
			{
				Out.Add(obj);
			}
		}
	}

	float cosh(float x)
	{
		return (FMath::Pow(2.71828f, x) + FMath::Pow(2.71828f, -x)) / 2;
	};

	float tanh(float x)
	{
		float k = FMath::Pow(2.71828f, 2 * x);
		return (k - 1) / (k + 1);
	};

	void RadiusRandom2D(float radius, float &x, float &y);

	int32 GetCurrentInputMode(const APlayerController *PlayerController);
		
	uint32 GetIPAddr();

	void PlaySnd(UAudioComponent* AudioComponent, USoundCue* SoundCue);
};

extern Util UTIL;
